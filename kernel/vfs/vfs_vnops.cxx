/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <ioctl.h>
#include <string.h>
#include <sys/process.hxx>
#include <sys/stat.h>
#include <sys/uio.hxx>
#include <unique/log.hxx>
#include <vfs/file.hxx>
#include <vfs/mount.hxx>
#include <vfs/vfs_lookup.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vattr.hxx>
#include <vfs/vfs_vnode.hxx>
#include <vfs/vfs_vnops.hxx>

namespace VFS {
    /*
     * Common code for vnode open operations.
     * Check permissions, and call the VOP_OPEN or VOP_CREATE routine.
     */
    int VNode::vn_open(Nameidata &ndp, Sys::Process &p, int fmode, int cmode) {
        VNode *vp;
        VAttr vap;
        int error;

        if ((fmode & (FREAD | FWRITE)) == 0) {
            return EINVAL;
        }

        if (fmode & O_CREAT) {
            ndp.ni_nameiop = CREATE | LOCKPARENT | LOCKLEAF;

            if ((fmode & O_EXCL) == 0) {
                ndp.ni_nameiop |= FOLLOW;
            }

            error = namei(ndp, p);
            if (error != 0) {
                return error;
            }

            if (ndp.ni_vp == nullptr) {
                // create file
                vattr_null(&vap);
                vap.va_type = VREG;
                vap.va_mode = static_cast<unsigned short>(cmode);

                error = ndp.ni_dvp->ops->create(ndp, vap, p);
                if (error != 0) {
                    vput(ndp.ni_dvp);
                    ndp.ni_dvp = nullptr;
                    return error;
                }

                vp = ndp.ni_vp;
                if (vp == nullptr) {
                    vput(ndp.ni_dvp);
                    ndp.ni_dvp = nullptr;
                    return EINVAL;
                }

                // no need to save parent directory
                if (ndp.ni_dvp != vp) {
                    vput(ndp.ni_dvp);
                } else {
                    vrele(ndp.ni_dvp);
                }

                ndp.ni_dvp = nullptr;

                fmode &= ~O_CREAT;
                fmode &= ~O_TRUNC;
            } else {
                // EXIsts
                ndp.ni_dvp->ops->abortop(ndp);

                if (ndp.ni_dvp == ndp.ni_vp) {
                    vrele(ndp.ni_dvp);
                } else {
                    vput(ndp.ni_dvp);
                }

                ndp.ni_dvp = nullptr;

                vp = ndp.ni_vp;

                if (fmode & O_EXCL) {
                    vput(vp);
                    return EEXIST;
                }

                fmode &= ~O_CREAT;
            }
        } else {
            ndp.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;

            error = namei(ndp, p);
            if (error != 0) {
                return error;
            }

            vp = ndp.ni_vp;
        }

        if (vp->v_type == VSOCK) {
            vput(vp);
            return EOPNOTSUPP;
        }

        if ((fmode & O_CREAT) == 0) {
            if (fmode & FREAD) {
                error = vp->ops->access(
                        *vp,
                        VREAD,
                        p.get_user_credentials(),
                        p);

                if (error != 0) {
                    vput(vp);
                    return error;
                }
            }

            if (fmode & (FWRITE | O_TRUNC)) {
                if (vp->v_type == VDIR) {
                    vput(vp);
                    return EISDIR;
                }

                error = vp->writechk();
                if (error != 0) {
                    vput(vp);
                    return error;
                }

                error = vp->ops->access(
                        *vp,
                        VWRITE,
                        p.get_user_credentials(),
                        p);

                if (error != 0) {
                    vput(vp);
                    return error;
                }
            }
        }

        if (fmode & O_TRUNC) {
            vattr_null(&vap);
            vap.va_qsize = 0;

            error = vp->ops->setattr(
                    *vp,
                    vap,
                    p.get_user_credentials(),
                    p);

            if (error != 0) {
                vput(vp);
                return error;
            }
        }

        error = vp->ops->open(
                *vp,
                fmode,
                p.get_user_credentials(),
                p);

        if (error != 0) {
            vput(vp);
            return error;
        }

        if (fmode & FWRITE) {
            vp->v_writecount++;
        }

        ndp.ni_vp = vp;

        if (vp->ops != nullptr) {
            vp->ops->unlock(*vp);
        }

        return 0;
    }

    /*
     * Check for write permissions on the specified vnode.
     * The read-only status of the file system is checked.
     * Also, prototype text segments cannot be written.
     */
    int VNode::writechk() const {
        if (v_mount != nullptr && (v_mount->mnt_flag & MNT_RDONLY)) {
            switch (v_type) {
                case VREG:
                case VDIR:
                case VLNK:
                    return EROFS;
                default:
                    break;
            }
        }
        if (v_flag & VTEXT) {
            return ETXTBSY;
        }
        return 0;
    }

    /*
     * Vnode close call.
     */
    int VNode::vn_close(int flags, const Sys::UserCredentials &cred,
                        Sys::Process &p) {
        if (flags & FWRITE) {
            v_writecount--;
        }
        int error = ops->close(*this, flags, cred, p);
        vrele(this);
        return error;
    }

    /*
     * Package up an I/O request on a vnode into a uio and do it.
     */
    int VNode::rdwr(enum uio_rw rw, void *base, int len, off_t offset,
                    enum uio_seg segflg, int ioflg,
                    const Sys::UserCredentials &cred, int *aresid,
                    Sys::Process &p) {
        struct iovec aiov = {base, len};
        struct uio auio = {&aiov, 1, offset, len, segflg, rw, p};
        int error;

        if ((ioflg & IO_NODELOCKED) == 0) {
            ops->lock(*this);
        }
        if (rw == UIO_READ) {
            error = ops->read(*this, &auio, ioflg, cred);
        } else {
            error = ops->write(*this, &auio, ioflg, cred);
        }
        if (aresid != nullptr) {
            *aresid = auio.uio_resid;
        } else {
            if (auio.uio_resid != 0 && error == 0) {
                error = EIO;
            }
        }
        if ((ioflg & IO_NODELOCKED) == 0) {
            ops->unlock(*this);
        }
        return error;
    }

    /*
     * File table vnode read routine.
     */
    int VNode::vn_read(File *fp, struct uio *uio,
                       const Sys::UserCredentials &cred) {
        if ((fp->f_flag & FREAD) == 0) {
            return EBADF;
        }
        ops->lock(*this);
        uio->uio_offset = fp->f_offset;
        int count = uio->uio_resid;
        int error = ops->read(*this, uio,
                              (fp->f_flag & FNONBLOCK) ? IO_NDELAY : 0, cred);
        fp->f_offset += count - uio->uio_resid;
        ops->unlock(*this);
        return error;
    }

    /*
     * File table vnode write routine.
     */
    int VNode::vn_write(File *fp, struct uio *uio,
                        const Sys::UserCredentials &cred) {
        if ((fp->f_flag & FWRITE) == 0) {
            return EBADF;
        }
        int count;
        int error;
        int ioflag = 0;

        if (v_type == VREG && (fp->f_flag & O_APPEND)) {
            ioflag |= IO_APPEND;
        }
        if (fp->f_flag & FNONBLOCK) {
            ioflag |= IO_NDELAY;
        }

        ops->lock(*this);
        uio->uio_offset = fp->f_offset;
        count = uio->uio_resid;
        error = ops->write(*this, uio, ioflag, cred);
        if (ioflag & IO_APPEND) {
            fp->f_offset = uio->uio_offset;
        } else {
            fp->f_offset += count - uio->uio_resid;
        }
        ops->unlock(*this);
        return error;
    }

    /*
     * File table vnode stat routine.
     */
    int VNode::vn_stat(struct stat *sb, Sys::Process &p) {
        VAttr vap;
        int error = ops->getattr(*this, vap, p.get_user_credentials());
        if (error != 0) {
            return error;
        }

        memset(sb, 0, sizeof(*sb));
        sb->st_dev = vap.va_fsid;
        sb->st_ino = vap.va_fileid;

        mode_t mode = vap.va_mode;
        switch (v_type) {
            case VREG:
                mode |= S_IFREG;
                break;
            case VDIR:
                mode |= S_IFDIR;
                break;
            case VBLK:
                mode |= S_IFBLK;
                break;
            case VCHR:
                mode |= S_IFCHR;
                break;
            case VLNK:
                mode |= S_IFLNK;
                break;
            case VSOCK:
                mode |= S_IFSOCK;
                break;
            case VFIFO:
                mode |= S_IFIFO;
                break;
            default:
                return EBADF;
        }
        sb->st_mode = mode;
        sb->st_nlink = vap.va_nlink;
        sb->st_uid = vap.va_uid;
        sb->st_gid = vap.va_gid;
        sb->st_rdev = vap.va_rdev;
        sb->st_size = vap.va_qsize;
        sb->st_atime = vap.va_atime.tv_sec;
        sb->st_mtime = vap.va_mtime.tv_sec;
        sb->st_ctime = vap.va_ctime.tv_sec;
        sb->st_blksize = vap.va_blocksize;
        sb->st_blocks = vap.va_qbytes / S_BLKSIZE;
        sb->st_flags = vap.va_flags;
        sb->st_gen = vap.va_gen;
        return 0;
    }

    /*
     * File table vnode ioctl routine.
     */
    int VNode::vn_ioctl(File *fp, unsigned int com, void *data,
                        Sys::Process &p) {
        VAttr vattr;
        int error;

        switch (v_type) {
            case VREG:
            case VDIR:
                if (com == FIONREAD) {
                    error = ops->getattr(*this, vattr, p.get_user_credentials());
                    if (error != 0) {
                        return error;
                    }
                    *static_cast<off_t *>(data) =
                            static_cast<off_t>(vattr.va_qsize) - fp->f_offset;
                    return 0;
                }
                if (com == FIONBIO || com == FIOASYNC) {
                    return 0;
                }
                [[fallthrough]];
            default:
                return ENOTTY;
            case VFIFO:
            case VCHR:
            case VBLK:
                error = ops->ioctl(*this, com, data, fp->f_flag,
                                   p.get_user_credentials(), p);
                return error;
        }
    }

    /*
     * File table vnode select routine.
     */
    int VNode::vn_select(File *fp, int which, Sys::Process &p) {
        return ops->select(*this, which, fp->f_flag, fp->f_cred, p);
    }

    /*
     * File table vnode close routine.
     */
    int VNode::vn_closefile(File *fp, Sys::Process &p) {
        return vn_close(fp->f_flag, fp->f_cred, p);
    }

    // Convert a file handle to a vnode pointer.
    int VNode::fhtovp(fhandle_t *fhp, int lockflag) {
        (void) fhp;
        if (!lockflag) {
            ops->unlock(*this);
        }
        return 0;
    }

    class VNodeFileOps final : public FileOps {
    public:
        int read(File &fp, struct uio &uio, const Sys::UserCredentials &cred) override {
            if (fp.f_type != DTYPE_VNODE || fp.f_data == nullptr) {
                return EINVAL;
            }

            auto *vp = static_cast<VNode *>(fp.f_data);
            return vp->vn_read(&fp, &uio, cred);
        }

        int write(File &fp, struct uio &uio, const Sys::UserCredentials &cred) override {
            if (fp.f_type != DTYPE_VNODE || fp.f_data == nullptr) {
                return EINVAL;
            }

            auto *vp = static_cast<VNode *>(fp.f_data);
            return vp->vn_write(&fp, &uio, cred);
        }

        int ioctl(File &fp, const unsigned int com, void *data, Sys::Process &p) override {
            if (fp.f_type != DTYPE_VNODE || fp.f_data == nullptr) {
                return EINVAL;
            }

            auto *vp = static_cast<VNode *>(fp.f_data);
            return vp->vn_ioctl(&fp, com, data, p);
        }

        int select(File &fp, const int which, Sys::Process &p) override {
            if (fp.f_type != DTYPE_VNODE || fp.f_data == nullptr) {
                return EINVAL;
            }

            auto *vp = static_cast<VNode *>(fp.f_data);
            return vp->vn_select(&fp, which, p);
        }

        int close(File &fp, Sys::Process &p) override {
            if (fp.f_data == nullptr) {
                return 0;
            }

            if (fp.f_type != DTYPE_VNODE) {
                fp.f_data = nullptr;
                return EINVAL;
            }

            auto *vp = static_cast<VNode *>(fp.f_data);

            const int error = vp->vn_closefile(&fp, p);

            fp.f_data = nullptr;

            return error;
        }
    };

    VNodeFileOps vnode_fileops_instance;

    FileOps &vn_fileops = vnode_fileops_instance;

} // namespace VFS
