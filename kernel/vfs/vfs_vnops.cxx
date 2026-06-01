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
//  *
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
 *	@(#)vfs_vnops.cxx
 */

#include <errno.h>
#include <fcntl.h>
#include <ioctl.h>
#include <stdint.h>
#include <sys/process.hxx>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.hxx>
#include <vfs/file.h>
#include <vfs/mount.h>
#include <vfs/namei.h>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_vattr.hxx>
#include <vfs/vfs_vnode.hxx>
#include <vfs/vfs_vnops.hxx>
#include <vfs/vnode.h>

namespace VFS {
    /*
     * Common code for vnode open operations.
     * Check permissions, and call the VOP_OPEN or VOP_CREATE routine.
     */
    int VNode::open(VNode *&vp, Nameidata &ndp, Sys::Process &p, int fmode, int cmode) {
        Sys::UserCredentials cred;
        VAttr vap;
        int error;
        // No namei caching right now.....

        if (fmode & O_CREAT) {
            ndp.ni_nameiop = CREATE | LOCKPARENT | LOCKLEAF;
            if ((fmode & O_EXCL) == 0)
                ndp.ni_nameiop |= FOLLOW;
            // if (error = namei(ndp, p))
            //     return (error);
            if (ndp.ni_vp == nullptr) {
                vap.va_type = VREG;
                vap.va_mode = cmode;
                if ((error = vp->ops->create(ndp, vap, p)))
                    return error;
                fmode &= ~O_TRUNC;
                vp = ndp.ni_vp;
            } else {
                vp->ops->aborttop(ndp);
                // if (ndp.ni_dvp == ndp.ni_vp)
                //  vrele(ndp.ni_dvp);
                //  release this node
                // else
                //  vput(ndp.ni_dvp);
                //  opposite
                ndp.ni_dvp = nullptr;
                vp = ndp.ni_vp;
                if (fmode & O_EXCL) {
                    error = EEXIST;
                    // vput(vp);
                    return error;
                }
                fmode &= ~O_CREAT;
            }
        } else {
            ndp.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
            // if (error = namei(ndp, p))
            //     return (error);
            vp = ndp.ni_vp;
        }
        if (vp->v_type == VSOCK) {
            error = EOPNOTSUPP;
            // vput(vp);
            return error;
        }
        if ((fmode & O_CREAT) == 0) {
            if (fmode & FREAD) {
                if ((error = vp->ops->acess(*vp, VREAD, cred, p))) {
                    // vput(vp);
                    return error;
                }
            }
            if (fmode & (FWRITE | O_TRUNC)) {
                if (vp->v_type == VDIR) {
                    error = EISDIR;
                    // vput(vp);
                    return error;
                }
                if (((error = vp->writechk())) ||
                    ((error = vp->ops->acess(*vp, VWRITE, cred, p)))) {
                    // vput(vp);
                    return error;
                }
            }
        }
        if (fmode & O_TRUNC) {
            vap.va_qsize = 0;
            if ((error = vp->ops->setattr(*vp, vap, cred, p))) {
                // vput(vp);
                return error;
            }
        }
        if ((error = vp->ops->open(*vp, fmode, cred, p))) {
            // vput(vp);
            return error;
        }

        if (fmode & FWRITE)
            vp->v_writecount++;
        return 0;
    };

    /*
     * Check for write permissions on the specified vnode.
     * The read-only status of the file system is checked.
     * Also, prototype text segments cannot be written.
     */
    int VNode::writechk() const {
        /*
         * Disallow write attempts on read-only file systems;
         * unless the file is a socket or a block or character
         * device resident on the file system.
         */
        if (v_mount->mnt_flag & MNT_RDONLY) {
            switch (v_type) {
                case VREG:
                case VDIR:
                case VLNK:
                    return EROFS;
                default:
                    break;
            };
        }

        /*
         * If there's shared text associated with
         * the vnode, try to free it up once.  If
         * we fail, we can't allow writing.
         */
        if ((v_flag & VTEXT) /* &&  !vnode_pager_uncache() */) { // from vnode pager!!!
            return ETXTBSY;
        };
        return 0;
    };

    // vnode close call
    int VNode::close(int flags, Sys::UserCredentials &cred, Sys::Process &p) {
        if (flags & FWRITE) {
            v_writecount--;
        }
        int error = ops->close(*this, flags, cred, p);
        // TODO: release 'this'.
        return error;
    }

    /*
     * Package up an I/O request on a vnode into a uio and do it.
     */
    int VNode::rdwr(enum uio_rw rw, void *base, int len, off_t offset, enum uio_seg segflg, int ioflg, Sys::UserCredentials &cred, int *aresid, Sys::Process &p) {
        struct iovec aiov{base, len};
        struct uio auio{&aiov, 1, offset,
                        len, segflg, rw, p};
        int error;

        if ((ioflg & IO_NODELOCKED) == 0) {
            ops->lock(*this);
        }

        if (rw == UIO_READ) {
            error = ops->read(*this, &auio, ioflg, cred);
        } else {
            error = ops->write(*this, &auio, ioflg, cred);
        }

        if (aresid) {
            *aresid = auio.uio_resid;
        } else {
            if (auio.uio_resid && error == 0) {
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
    int VNode::read(struct file *fp, struct uio *uio, Sys::UserCredentials &cred) {
        // struct vnode *vp = (struct vnode *)fp->f_data;,
        // use for future, when sockets are added
        ops->lock(*this);
        uio->uio_offset = fp->f_offset;
        int count = uio->uio_resid;
        int error = ops->read(*this, uio, (fp->f_flag & FNONBLOCK) ? IO_NDELAY : 0, cred);
        fp->f_offset += count - uio->uio_resid;
        ops->unlock(*this);
        return error;
    }

    /*
     * File table vnode write routine.
     */
    int VNode::write(struct file *fp, struct uio *uio, Sys::UserCredentials &cred) {
        // struct vnode *vp = (struct vnode *) fp->f_data;
        int count, error, ioflag = 0;

        if (v_type == VREG && (fp->f_flag & O_APPEND))
            ioflag |= IO_APPEND;
        if (fp->f_flag & FNONBLOCK)
            ioflag |= IO_NDELAY;
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
    int VNode::stat(struct stat *sb, Sys::Process &p) {
        VAttr vap{};

        int error = ops->getattr(*this, vap,
                                 const_cast<Sys::UserCredentials &>(p.get_user_credentials()));
        if (error)
            return error;

        /*
         * Copy from vattr table
         */
        sb->st_dev = vap.va_fsid;
        sb->st_ino = vap.va_fileid;
        uint16_t mode = vap.va_mode;
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
        };
        sb->st_mode = mode;
        sb->st_nlink = vap.va_nlink;
        sb->st_uid = vap.va_uid;
        sb->st_gid = vap.va_gid;
        sb->st_rdev = vap.va_rdev;
        sb->st_size = vap.va_qsize;
        sb->st_atime = vap.va_atime.tv_sec;
        sb->st_spare1 = 0;
        sb->st_mtime = vap.va_mtime.tv_sec;
        sb->st_spare2 = 0;
        sb->st_ctime = vap.va_ctime.tv_sec;
        sb->st_spare3 = 0;
        sb->st_blksize = vap.va_blocksize;
        sb->st_flags = vap.va_flags;
        sb->st_gen = vap.va_gen;
        sb->st_blocks = vap.va_qbytes / S_BLKSIZE;
        return 0;
    }

    /*
     * File table vnode ioctl routine.
     */
    int VNode::ioctl(struct file *fp, unsigned int com, void *data, Sys::Process &p) {
        VAttr vattr;
        int error;

        switch (v_type) {

            case VREG:
            case VDIR:
                if (com == FIONREAD) {
                    error = ops->getattr(*this, vattr, const_cast<Sys::UserCredentials &>(p.get_user_credentials()));
                    if (error)
                        return error;
                    *(off_t *) data = vattr.va_qsize - fp->f_offset;
                    return 0;
                }
                if (com == FIONBIO || com == FIOASYNC) /* XXX */
                    return 0; /* XXX */
                /* fall into ... */

            default:
                return ENOTTY;

            case VFIFO:
            case VCHR:
            case VBLK:
                error = ops->ioctl(*this, com, data, fp->f_flag,
                                   const_cast<Sys::UserCredentials &>(p.get_user_credentials()), p);

#ifdef STUBBED // Cool for init and programs, that can capture tty. Not now.
                if (error == 0 && com == TIOCSCTTY) {
                    p->p_session->s_ttyvp = vp;
                    v_usecount++;
                }
#endif // STUBBED
                return error;
        }
    }

    /*
     * File table vnode select routine.
     */
    int VNode::select(struct file *fp, int which, Sys::Process &p) {

        return ops->select(*this, which, fp->f_flag,
                           fp->f_cred, p);
    }

    /*
     * File table vnode close routine.
     */
    int VNode::closefile(struct file *fp, Sys::Process &p) {

        return close(fp->f_flag, fp->f_cred, p);
    }

    /*
     * vn_fhtovp() - convert a fh to a vnode ptr (optionally locked)
     * 	- look up fsid in mount list (if not found ret error)
     *	- get vp by calling VFS_FHTOVP() macro
     *	- if lockflag lock it with VOP_LOCK()
     */
    int VNode::fhtovp(fhandle_t *fhp, int lockflag) {
        // struct mount *mp;

        // getvfs -- part of subr
        // if ((mp = getvfs(&fhp->fh_fsid)) == NULL)
        //     return (ESTALE);

        // second stub in one function, sigh.
        // if (VFS_FHTOVP(mp, &fhp->fh_fid, *this))
        //    return (ESTALE);
        (void) fhp;

        if (!lockflag)
            ops->unlock(*this);
        return 0;
    }
} // namespace VFS
