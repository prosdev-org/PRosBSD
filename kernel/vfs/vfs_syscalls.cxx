/*
 * Copyright (c) 1989 The Regents of the University of California.
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
#include <libkxx/heap.hxx>
#include <limits.h>
#include <string.h>
#include <sys/panic.hxx>
#include <sys/process.hxx>
#include <sys/stat.h>
#include <sys/uio.hxx>
#include <unique/log.hxx>
#include <unistd.h>
#include <vfs/file.hxx>
#include <vfs/mount.hxx>
#include <vfs/name_cache.hxx>
#include <vfs/vfs_lookup.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_syscalls.hxx>
#include <vfs/vfs_vattr.hxx>
#include <vfs/vfs_vnode.hxx>

namespace VFS {
    // is su TODO: ITS HACK
    static int suser(const Sys::UserCredentials &cred) {
        if (cred.user_id == 0) {
            return 0;
        }
        return EPERM;
    }

    // descriptor to kernel id
    int getvnode(FileContext &fdp, int fdes, File *&fpp) {
        if (fdes < 0 || fdes >= fdp.fd_nfiles) {
            return EBADF;
        }
        File *fp = fdp.fd_ofiles[fdes];
        if (fp == nullptr) {
            return EBADF;
        }
        if (fp->f_type != DTYPE_VNODE) {
            return EINVAL;
        }
        fpp = fp;
        return 0;
    }

    // chroot and chdir
    int chdirec(Nameidata &ndp, Sys::Process &p) {
        VNode *vp;
        int error;

        if ((error = namei(ndp, p)) != 0) {
            return error;
        }
        vp = ndp.ni_vp;
        if (vp->v_type != VDIR) {
            error = ENOTDIR;
        } else {
            error = vp->ops->access(*vp, VEXEC, p.get_user_credentials(), p);
        }
        if (vp->ops != nullptr) {
            vp->ops->unlock(*vp);
        }
        if (error != 0) {
            vrele(vp);
        }
        return error;
    }

    // allocate file and descriptor
    static int falloc(Sys::Process &p, File *&nfp, int &indx) {
        FileContext &fdp = p.get_file_context();
        int error = fdp.alloc_fd(indx);
        if (error != 0) {
            return error;
        }
        File *fp = new File();
        fp->f_count = 1;
        fp->f_cred = p.get_user_credentials();
        fdp.fd_ofiles[indx] = fp;
        nfp = fp;
        return 0;
    }

    /*
     * Mount system call.
     */
    int sys_mount(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct mount_args {
            int type;
            char *dir;
            int flags;
            void *data;
        };
        auto *args = static_cast<struct mount_args *>(uap);
        int error;

        if ((error = suser(p.get_user_credentials())) != 0) {
            return error;
        }

        /* Get vnode to be covered */
        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->dir;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }
        VNode *vp = nd.ni_vp;
        Mount *mp = nullptr;

        if (args->flags & MNT_UPDATE) {
            if ((vp->v_flag & VROOT) == 0) {
                vput(vp);
                return EINVAL;
            }
            mp = vp->v_mount;
            mp->mnt_flag |= MNT_UPDATE;
            vp->ops->unlock(*vp);
            goto update;
        }

        if (vp->v_usecount != 1) {
            vput(vp);
            return EBUSY;
        }
        if (vp->v_type != VDIR) {
            vput(vp);
            return ENOTDIR;
        }
        if (args->type < 0 || args->type > MOUNT_MAXTYPE ||
            vfssw[args->type] == nullptr) {
            vput(vp);
            return ENODEV;
        }

        /* Allocate and initialize mount */
        mp = new Mount();
        mp->mnt_op = vfssw[args->type];
        mp->mnt_flag = 0;

        if ((error = vfs_lock(mp)) != 0) {
            delete mp;
            vput(vp);
            return error;
        }

        vp->v_mount = mp;
        mp->mnt_vnodecovered = vp;

    update:
        /* Set the mount level flags */
        if (args->flags & MNT_RDONLY) {
            mp->mnt_flag |= MNT_RDONLY;
        } else {
            mp->mnt_flag &= ~MNT_RDONLY;
        }
        if (args->flags & MNT_NOSUID) {
            mp->mnt_flag |= MNT_NOSUID;
        } else {
            mp->mnt_flag &= ~MNT_NOSUID;
        }
        if (args->flags & MNT_NOEXEC) {
            mp->mnt_flag |= MNT_NOEXEC;
        } else {
            mp->mnt_flag &= ~MNT_NOEXEC;
        }

        /*
         * Mount the filesystem
         */
        error = VFS_MOUNT(mp, args->dir, args->data, &nd, p);

        if (mp->mnt_flag & MNT_UPDATE) {
            mp->mnt_flag &= ~MNT_UPDATE;
            vrele(vp);
            return error;
        }

        /*
         * Put the new filesystem on the mount list after root.
         */
        if (rootfs != nullptr) {
            mp->mnt_next = rootfs->mnt_next;
            mp->mnt_prev = rootfs;
            rootfs->mnt_next = mp;
            if (mp->mnt_next != nullptr) {
                mp->mnt_next->mnt_prev = mp;
            }
        } else {
            rootfs = mp;
            mp->mnt_next = mp;
            mp->mnt_prev = mp;
        }

        NameCache::purge(*vp);

        if (error == 0) {
            vp->ops->unlock(*vp);
            vfs_unlock(mp);
            error = VFS_START(mp, 0, p);
            if (error != 0) {
                (void) VFS_UNMOUNT(mp, MNT_FORCE, p);
                vfs_remove(mp);
                delete mp;
            }
        } else {
            vfs_remove(mp);
            delete mp;
            vput(vp);
        }
        return error;
    }

    /*
     * Unmount system call.
     */
    int sys_unmount(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct unmount_args {
            char *pathp;
            int flags;
        };
        auto *args = static_cast<struct unmount_args *>(uap);
        int error;

        if ((error = suser(p.get_user_credentials())) != 0) {
            return error;
        }

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | LOCKLEAF | FOLLOW;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->pathp;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if ((vp->v_flag & VROOT) == 0) {
            vput(vp);
            return EINVAL;
        }
        Mount *mp = vp->v_mount;
        vput(vp);
        return dounmount(mp, args->flags, p);
    }

    /*
     * Do an unmount.
     */
    int dounmount(Mount *mp, int flags, Sys::Process &p) {
        VNode *coveredvp = mp->mnt_vnodecovered;
        int error;

        if (vfs_busy(mp) != 0) {
            return EBUSY;
        }
        mp->mnt_flag |= MNT_UNMOUNT;
        if ((error = vfs_lock(mp)) != 0) {
            mp->mnt_flag &= ~MNT_UNMOUNT;
            vfs_unbusy(mp);
            return error;
        }

        NameCache::purge_vfs(*mp);

        if ((error = VFS_SYNC(mp, MNT_WAIT)) == 0 || (flags & MNT_FORCE)) {
            error = VFS_UNMOUNT(mp, flags, p);
        }
        mp->mnt_flag &= ~MNT_UNMOUNT;
        vfs_unbusy(mp);

        if (error != 0) {
            vfs_unlock(mp);
        } else {
            vfs_unlock(mp);
            vrele(coveredvp);
            vfs_remove(mp);
            delete mp;
        }
        return error;
    }

    /*
     * Sync system call.
     * Sync each mounted filesystem.
     */
    int sys_sync(Sys::Process &p, void *uap, int *retval) {
        (void) p;
        (void) uap;
        (void) retval;

        Mount *mp = rootfs;
        if (mp == nullptr) {
            return 0;
        }
        do { // dowhile usecase?
            if ((mp->mnt_flag & (MNT_MLOCK | MNT_RDONLY | MNT_MPBUSY)) == 0 &&
                !vfs_busy(mp)) {
                VFS_SYNC(mp, MNT_NOWAIT);
                Mount *omp = mp;
                mp = mp->mnt_next;
                vfs_unbusy(omp);
            } else {
                mp = mp->mnt_next;
            }
        } while (mp != rootfs);
        return 0;
    }

    /*
     * Get filesystem statistics.
     */
    int sys_statfs(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct statfs_args {
            char *path;
            struct statfs *buf;
        };
        auto *args = static_cast<struct statfs_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->path;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }
        Mount *mp = nd.ni_vp->v_mount;
        vrele(nd.ni_vp);

        struct statfs *sp = &mp->mnt_stat;
        if ((error = VFS_STATFS(mp, sp, p)) != 0) {
            return error;
        }
        sp->f_flags = mp->mnt_flag & MNT_VISFLAGMASK;
        memmove(args->buf, sp, sizeof(*sp));
        return 0;
    }

    /*
     * Change current working directory (``.'').
     */
    int sys_chdir(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct chdir_args {
            char *fname;
        };
        auto *args = static_cast<struct chdir_args *>(uap);
        FileContext &fdp = p.get_file_context();
        int error;

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        if ((error = chdirec(nd, p)) != 0) {
            return error;
        }
        if (fdp.fd_cdir != nullptr) {
            vrele(fdp.fd_cdir);
        }
        fdp.fd_cdir = nd.ni_vp;
        return 0;
    }

    /*
     * Change notion of root (``/'') directory.
     */
    int sys_chroot(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct chroot_args {
            char *fname;
        };
        auto *args = static_cast<struct chroot_args *>(uap);
        FileContext &fdp = p.get_file_context();
        int error;

        if ((error = suser(p.get_user_credentials())) != 0) {
            return error;
        }

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        if ((error = chdirec(nd, p)) != 0) {
            return error;
        }
        if (fdp.fd_rdir != nullptr) {
            vrele(fdp.fd_rdir);
        }
        fdp.fd_rdir = nd.ni_vp;
        return 0;
    }

    /*
     * Open system call.
     */
    int sys_open(Sys::Process &p, void *uap, int *retval) {
        auto *args = static_cast<struct open_args *>(uap);

        FileContext &fdp = p.get_file_context();
        File *nfp = nullptr;
        int indx = -1;

        int error = falloc(p, nfp, indx);
        if (error != 0) {
            return error;
        }

        const int fmode = FFLAGS(args->mode);
        const int cmode = ((args->crtmode & ~fdp.fd_cmask) & 07777) & ~S_ISVTX;

        Nameidata nd;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;

        error = VNode::vn_open(nd, p, fmode, cmode);
        if (error != 0) {
            delete nfp;
            fdp.fd_ofiles[indx] = nullptr;
            return error;
        }

        VNode *vp = nd.ni_vp;

        nfp->f_flag = fmode & (FREAD | FWRITE);
        if (fmode & O_APPEND) {
            nfp->f_flag |= O_APPEND;
        }
        if (fmode & O_NONBLOCK) {
            nfp->f_flag |= O_NONBLOCK;
        }

        nfp->f_type = DTYPE_VNODE;
        nfp->f_ops = &vn_fileops;
        nfp->f_data = vp;
        nfp->f_offset = 0;

        *retval = indx;

        return 0;
    }

    /*
     * Creat system call.
     */
    int sys_creat(Sys::Process &p, void *uap, int *retval) {
        struct creat_args {
            char *fname;
            int fmode;
        };
        auto *args = static_cast<struct creat_args *>(uap);

        struct open_args openuap;
        openuap.fname = args->fname;
        openuap.crtmode = args->fmode;
        openuap.mode = O_WRONLY | O_CREAT | O_TRUNC;
        return sys_open(p, &openuap, retval);
    }

    /*
     * Mkdir system call.
     */
    int sys_mkdir(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct mkdir_args {
            char *name;
            int dmode;
        };

        auto *args = static_cast<struct mkdir_args *>(uap);
        FileContext &fdp = p.get_file_context();

        Nameidata nd;
        nd.ni_nameiop = CREATE | LOCKPARENT;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->name;

        int error = namei(nd, p);
        if (error != 0) {
            return error;
        }

        if (nd.ni_vp != nullptr) {
            nd.ni_dvp->ops->abortop(nd);

            if (nd.ni_dvp == nd.ni_vp) {
                vrele(nd.ni_dvp);
            } else {
                vput(nd.ni_dvp);
                vrele(nd.ni_vp);
            }

            return EEXIST;
        }

        VAttr vattr;
        vattr_null(&vattr);
        vattr.va_type = VDIR;
        vattr.va_mode = static_cast<mode_t>((args->dmode & 0777) & ~fdp.fd_cmask);

        error = nd.ni_dvp->ops->mkdir(nd, vattr, p);

        if (error == 0) {
            if (nd.ni_vp != nullptr) {
                vput(nd.ni_vp);
            }

            vput(nd.ni_dvp);
        } else {
            nd.ni_dvp->ops->abortop(nd);
            vput(nd.ni_dvp);
        }

        return error;
    }

    /*
     * Rmdir system call.
     */
    int sys_rmdir(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct rmdir_args {
            char *name;
        };

        auto *args = static_cast<struct rmdir_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = DELETE | LOCKPARENT | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->name;

        int error = namei(nd, p);
        if (error != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        VNode *dvp = nd.ni_dvp;

        if (vp->v_type != VDIR) {
            error = ENOTDIR;
        } else if (dvp == vp) {
            error = EINVAL;
        } else if (vp->v_flag & VROOT) {
            error = EBUSY;
        } else if (vp->v_mount != nullptr && (vp->v_mount->mnt_flag & MNT_RDONLY)) {
            error = EROFS;
        }

        if (error == 0) {
            error = dvp->ops->rmdir(nd, p);
        }

        if (error == 0) {
            if (dvp == vp) {
                vput(vp);
            } else {
                vput(vp);
                vput(dvp);
            }
        } else {
            dvp->ops->abortop(nd);

            if (dvp == vp) {
                vput(vp);
            } else {
                vput(vp);
                vput(dvp);
            }
        }

        return error;
    }

    /*
     * Unlink system call.
     */
    int sys_unlink(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct unlink_args {
            char *fname;
        };

        auto *args = static_cast<struct unlink_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = DELETE | LOCKPARENT | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;

        int error = namei(nd, p);
        if (error != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        VNode *dvp = nd.ni_dvp;

        if (vp->v_type == VDIR) {
            error = EPERM;
        } else if (vp->v_flag & VROOT) {
            error = EBUSY;
        } else if (vp->v_mount != nullptr && (vp->v_mount->mnt_flag & MNT_RDONLY)) {
            error = EROFS;
        }

        if (error == 0) {
            error = dvp->ops->remove(nd, p);
        }

        if (error == 0) {
            if (dvp == vp) {
                vput(vp);
            } else {
                vput(vp);
                vput(dvp);
            }
        } else {
            dvp->ops->abortop(nd);

            if (dvp == vp) {
                vput(vp);
            } else {
                vput(vp);
                vput(dvp);
            }
        }

        return error;
    }

    /*
     * Stat system call.
     */
    int sys_stat(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct stat_args {
            char *fname;
            struct stat *ub;
        };
        auto *args = static_cast<struct stat_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | LOCKLEAF | FOLLOW;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }
        struct stat sb;
        error = nd.ni_vp->vn_stat(&sb, p);
        vput(nd.ni_vp);
        if (error != 0) {
            return error;
        }
        memmove(args->ub, &sb, sizeof(sb));
        return 0;
    }

    /*
     * Lstat system call.
     */
    int sys_lstat(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct lstat_args {
            char *fname;
            struct stat *ub;
        };
        auto *args = static_cast<struct lstat_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | LOCKLEAF | NOFOLLOW;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }
        struct stat sb;
        error = nd.ni_vp->vn_stat(&sb, p);
        vput(nd.ni_vp);
        if (error != 0) {
            return error;
        }
        memmove(args->ub, &sb, sizeof(sb));
        return 0;
    }

    /*
     * Set the mode mask for creation of filesystem nodes.
     */
    int sys_umask(Sys::Process &p, void *uap, int *retval) {
        struct umask_args {
            int mask;
        };
        auto *args = static_cast<struct umask_args *>(uap);
        FileContext &fdp = p.get_file_context();
        *retval = fdp.fd_cmask;
        fdp.fd_cmask = args->mask & 07777;
        return 0;
    }

    /*
     * Link system call.
     */
    int sys_link(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct link_args {
            char *target;
            char *linkname;
        };
        auto *args = static_cast<struct link_args *>(uap);
        int error;

        Nameidata nd1;
        nd1.ni_nameiop = LOOKUP | FOLLOW;
        nd1.ni_segflg = UIO_SYSSPACE;
        nd1.ni_dirp = args->target;
        if ((error = namei(nd1, p)) != 0) {
            return error;
        }
        VNode *vp = nd1.ni_vp;

        Nameidata nd2;
        nd2.ni_nameiop = CREATE | LOCKPARENT;
        nd2.ni_segflg = UIO_SYSSPACE;
        nd2.ni_dirp = args->linkname;
        if ((error = namei(nd2, p)) != 0) {
            vrele(vp);
            return error;
        }

        if (nd2.ni_vp != nullptr) {
            error = EEXIST;
            goto out;
        }
        if (vp->v_mount != nd2.ni_dvp->v_mount) {
            error = EXDEV;
            goto out;
        }

    out:
        if (error == 0) {
            error = nd2.ni_dvp->ops->link(*vp, nd2, p);
            nd2.ni_dvp->ops->abortop(nd2);
            vput(nd2.ni_dvp);
            nd2.ni_dvp = nullptr;
        } else {
            nd2.ni_dvp->ops->abortop(nd2);
            if (nd2.ni_dvp == nd2.ni_vp) {
                vrele(nd2.ni_dvp);
            } else {
                vput(nd2.ni_dvp);
            }
            if (nd2.ni_vp != nullptr) {
                vrele(nd2.ni_vp);
            }
        }
        vrele(vp);
        return error;
    }

    /*
     * Symlink system call.
     */
    int sys_symlink(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct symlink_args {
            char *target;
            char *linkname;
        };
        auto *args = static_cast<struct symlink_args *>(uap);
        FileContext &fdp = p.get_file_context();

        Nameidata nd;
        nd.ni_nameiop = CREATE | LOCKPARENT;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->linkname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        if (nd.ni_vp != nullptr) {
            nd.ni_dvp->ops->abortop(nd);
            if (nd.ni_dvp == nd.ni_vp) {
                vrele(nd.ni_dvp);
            } else {
                vput(nd.ni_dvp);
            }
            vrele(nd.ni_vp);
            return EEXIST;
        }

        VAttr vattr;
        vattr_null(&vattr);
        vattr.va_mode = static_cast<unsigned short>(0777 & ~fdp.fd_cmask);

        error = nd.ni_dvp->ops->symlink(nd, vattr, args->target, p);
        nd.ni_dvp->ops->abortop(nd);
        vput(nd.ni_dvp);
        nd.ni_dvp = nullptr;
        return error;
    }

    /*
     * Readlink system call.
     */
    int sys_readlink(Sys::Process &p, void *uap, int *retval) {
        struct readlink_args {
            char *name;
            char *buf;
            int count;
        };
        auto *args = static_cast<struct readlink_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->name;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if (vp->v_type != VLNK) {
            vput(vp);
            return EINVAL;
        }

        struct iovec aiov = {args->buf, args->count};
        struct uio auio = {
                &aiov, 1, 0, args->count,
                UIO_SYSSPACE, UIO_READ, p};
        error = vp->ops->readlink(*vp, &auio, p.get_user_credentials());
        vput(vp);
        *retval = args->count - auio.uio_resid;
        return error;
    }

    /*
     * Chmod system call.
     */
    int sys_chmod(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct chmod_args {
            char *fname;
            int fmode;
        };
        auto *args = static_cast<struct chmod_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if (vp->v_mount->mnt_flag & MNT_RDONLY) {
            vput(vp);
            return EROFS;
        }

        VAttr vattr;
        vattr_null(&vattr);
        vattr.va_mode = static_cast<unsigned short>(args->fmode & 07777);
        error = vp->ops->setattr(*vp, vattr, p.get_user_credentials(), p);
        vput(vp);
        return error;
    }

    /*
     * Chown system call.
     */
    int sys_chown(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct chown_args {
            char *fname;
            int uid;
            int gid;
        };
        auto *args = static_cast<struct chown_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if (vp->v_mount->mnt_flag & MNT_RDONLY) {
            vput(vp);
            return EROFS;
        }

        VAttr vattr;
        vattr_null(&vattr);
        vattr.va_uid = args->uid;
        vattr.va_gid = args->gid;
        error = vp->ops->setattr(*vp, vattr, p.get_user_credentials(), p);
        vput(vp);
        return error;
    }

    /*
     * Truncate system call.
     */
    int sys_truncate(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct truncate_args {
            char *fname;
            off_t length;
        };
        auto *args = static_cast<struct truncate_args *>(uap);

        if (args->length < 0) {
            return EINVAL;
        }

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if (vp->v_type == VDIR) {
            vput(vp);
            return EISDIR;
        }
        if ((error = vp->writechk()) != 0 ||
            (error = vp->ops->access(*vp, VWRITE,
                                     p.get_user_credentials(), p)) != 0) {
            vput(vp);
            return error;
        }

        VAttr vattr;
        vattr_null(&vattr);
        vattr.va_qsize = args->length;
        error = vp->ops->setattr(*vp, vattr, p.get_user_credentials(), p);
        vput(vp);
        return error;
    }

    /*
     * Rename system call.
     */
    int sys_rename(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct rename_args {
            char *from;
            char *to;
        };
        auto *args = static_cast<struct rename_args *>(uap);

        Nameidata fromnd;
        fromnd.ni_nameiop = DELETE | WANTPARENT | SAVESTART;
        fromnd.ni_segflg = UIO_SYSSPACE;
        fromnd.ni_dirp = args->from;
        int error;
        if ((error = namei(fromnd, p)) != 0) {
            return error;
        }
        VNode *fvp = fromnd.ni_vp;

        Nameidata tond;
        tond.ni_nameiop = RENAME | LOCKPARENT | LOCKLEAF | NOCACHE | SAVESTART;
        tond.ni_segflg = UIO_SYSSPACE;
        tond.ni_dirp = args->to;
        if ((error = namei(tond, p)) != 0) {
            fromnd.ni_dvp->ops->abortop(fromnd);
            vrele(fromnd.ni_dvp);
            vrele(fvp);
            goto out1;
        }

        {
            VNode *tdvp = tond.ni_dvp;
            VNode *tvp = tond.ni_vp;

            if (tvp != nullptr) {
                if (fvp->v_type == VDIR && tvp->v_type != VDIR) {
                    error = ENOTDIR;
                    goto out;
                } else if (fvp->v_type != VDIR && tvp->v_type == VDIR) {
                    error = EISDIR;
                    goto out;
                }
            }
            if (fvp->v_mount != tdvp->v_mount) {
                error = EXDEV;
                goto out;
            }

        out:
            if (error == 0) {
                error = tdvp->ops->rename(fromnd, tond, p);
                tond.ni_dvp->ops->abortop(tond);
                if (tdvp == tvp) {
                    vrele(tdvp);
                } else {
                    vput(tdvp);
                }
                if (tvp != nullptr) {
                    vput(tvp);
                }
                fromnd.ni_dvp->ops->abortop(fromnd);
                vrele(fromnd.ni_dvp);
                vrele(fvp);
            } else {
                tond.ni_dvp->ops->abortop(tond);
                if (tdvp == tvp) {
                    vrele(tdvp);
                } else {
                    vput(tdvp);
                }
                if (tvp != nullptr) {
                    vput(tvp);
                }
                fromnd.ni_dvp->ops->abortop(fromnd);
                vrele(fromnd.ni_dvp);
                vrele(fvp);
            }
        }

    out1:
        return error;
    }

    /*
     * Mknod system call.
     */
    int sys_mknod(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct mknod_args {
            char *fname;
            int fmode;
            int dev;
        };
        auto *args = static_cast<struct mknod_args *>(uap);

        if (int error = suser(p.get_user_credentials()); error != 0) {
            return error;
        }

        Nameidata nd;
        nd.ni_nameiop = CREATE | LOCKPARENT;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        if (nd.ni_vp != nullptr) {
            nd.ni_dvp->ops->abortop(nd);
            if (nd.ni_dvp == nd.ni_vp) {
                vrele(nd.ni_dvp);
            } else {
                vput(nd.ni_dvp);
            }
            vrele(nd.ni_vp);
            return EEXIST;
        }

        VAttr vattr;
        vattr_null(&vattr);
        switch (args->fmode & S_IFMT) {
            case S_IFCHR:
                vattr.va_type = VCHR;
                break;
            case S_IFBLK:
                vattr.va_type = VBLK;
                break;
            default:
                nd.ni_dvp->ops->abortop(nd);
                vrele(nd.ni_dvp);
                return EINVAL;
        }
        FileContext &fdp = p.get_file_context();
        vattr.va_mode = static_cast<unsigned short>(
                (args->fmode & 07777) & ~fdp.fd_cmask);
        vattr.va_rdev = args->dev;

        error = nd.ni_dvp->ops->mknod(nd, vattr, p.get_user_credentials(), p);
        if (error != 0) {
            nd.ni_dvp->ops->abortop(nd);
        }
        vput(nd.ni_dvp);
        nd.ni_dvp = nullptr;
        return error;
    }

    /*
     * Fsync system call.
     */
    int sys_fsync(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct fsync_args {
            int fd;
        };
        auto *args = static_cast<struct fsync_args *>(uap);
        FileContext &fdp = p.get_file_context();
        File *fp;
        int error;

        if ((error = getvnode(fdp, args->fd, fp)) != 0) {
            return error;
        }

        VNode *vp = static_cast<VNode *>(fp->f_data);
        vp->ops->lock(*vp);
        error = vp->ops->fsync(*vp, fp->f_flag, fp->f_cred, MNT_WAIT, p);
        vp->ops->unlock(*vp);
        return error;
    }

    /*
     * Lseek system call.
     */
    int sys_lseek(Sys::Process &p, void *uap, int *retval) {
        struct lseek_args {
            int fdes;
            off_t off;
            int sbase;
        };

        auto *args = static_cast<struct lseek_args *>(uap);

        FileContext &fdp = p.get_file_context();
        File *fp = nullptr;

        int error = getvnode(fdp, args->fdes, fp);
        if (error != 0) {
            return error;
        }

        auto *vp = static_cast<VNode *>(fp->f_data);
        off_t newoff;

        if (vp->v_type != VREG) {
            return ESPIPE;
        }

        switch (args->sbase) {
            case L_SET:
                newoff = args->off;
                break;

            case L_INCR:
                if ((args->off > 0 && fp->f_offset > static_cast<off_t>(LONG_MAX) - args->off) ||
                    (args->off < 0 && fp->f_offset < static_cast<off_t>(LONG_MIN) - args->off)) {
                    return EFBIG;
                }
                newoff = fp->f_offset + args->off;
                break;

            case L_XTND: {
                VAttr vattr;
                error = vp->ops->getattr(*vp, vattr, p.get_user_credentials());
                if (error != 0) {
                    return error;
                }
                if (args->off > 0 && vattr.va_qsize > static_cast<unsigned long long>(LONG_MAX - args->off)) {
                    return EFBIG;
                }
                newoff = static_cast<off_t>(vattr.va_qsize) + args->off;
                break;
            }

            default:
                return EINVAL;
        }

        if (newoff < 0) {
            return EINVAL;
        }
        fp->f_offset = newoff;

        *retval = static_cast<int>(fp->f_offset);
        return 0;
    }

    /*
     * Access system call.
     */
    int sys_access(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct access_args {
            char *fname;
            int fmode;
        };
        auto *args = static_cast<struct access_args *>(uap);

        Nameidata nd;
        nd.ni_nameiop = LOOKUP | FOLLOW | LOCKLEAF;
        nd.ni_segflg = UIO_SYSSPACE;
        nd.ni_dirp = args->fname;
        int error;
        if ((error = namei(nd, p)) != 0) {
            return error;
        }

        VNode *vp = nd.ni_vp;
        if (args->fmode != 0) {
            int mode = 0;
            if (args->fmode & R_OK) {
                mode |= VREAD;
            }
            if (args->fmode & W_OK) {
                mode |= VWRITE;
            }
            if (args->fmode & X_OK) {
                mode |= VEXEC;
            }
            if ((mode & VWRITE) == 0 ||
                (error = vp->writechk()) == 0) {
                error = vp->ops->access(*vp, mode,
                                        p.get_user_credentials(), p);
            }
        }
        vput(vp);
        return error;
    }

    /*
     * Close system call.
     */
    int sys_close(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct close_args {
            int fd;
        };

        auto *args = static_cast<struct close_args *>(uap);
        FileContext &fdp = p.get_file_context();

        File *fp = fdp.get_file(args->fd);
        if (fp == nullptr) {
            return EBADF;
        }

        File *removed = fdp.remove_fd(args->fd);
        ASSERT(removed == fp);

        int error = 0;

        if (fp->f_ops != nullptr) {
            error = fp->f_ops->close(*fp, p);
        }

        delete fp;

        return error;
    }

    /*
     * Read system call.
     */
    int sys_read(Sys::Process &p, void *uap, int *retval) {
        struct read_args {
            int fd;
            void *buf;
            int count;
        };

        auto *args = static_cast<struct read_args *>(uap);

        if (args->count < 0) {
            return EINVAL;
        }

        FileContext &fdp = p.get_file_context();
        File *fp = nullptr;

        int error = getvnode(fdp, args->fd, fp);
        if (error != 0) {
            return error;
        }

        if (fp->f_ops == nullptr) {
            return EINVAL;
        }

        struct iovec aiov = {args->buf, args->count};
        struct uio auio = {
                &aiov,
                1,
                fp->f_offset,
                args->count,
                UIO_SYSSPACE,
                UIO_READ,
                p};

        error = fp->f_ops->read(*fp, auio, fp->f_cred);

        *retval = args->count - auio.uio_resid;

        return error;
    }

    /*
     * Write system call.
     */
    int sys_write(Sys::Process &p, void *uap, int *retval) {
        struct write_args {
            int fd;
            const void *buf;
            int count;
        };

        auto *args = static_cast<struct write_args *>(uap);

        if (args->count < 0) {
            return EINVAL;
        }

        FileContext &fdp = p.get_file_context();
        File *fp = nullptr;

        int error = getvnode(fdp, args->fd, fp);
        if (error != 0) {
            return error;
        }

        if (fp->f_ops == nullptr) {
            return EINVAL;
        }

        struct iovec aiov = {const_cast<void *>(args->buf), args->count};
        struct uio auio = {
                &aiov,
                1,
                fp->f_offset,
                args->count,
                UIO_SYSSPACE,
                UIO_WRITE,
                p};

        error = fp->f_ops->write(*fp, auio, fp->f_cred);

        *retval = args->count - auio.uio_resid;

        return error;
    }

    /*
     * Fstat system call.
     */
    int sys_fstat(Sys::Process &p, void *uap, int *retval) {
        (void) retval;

        struct fstat_args {
            int fd;
            struct stat *sb;
        };

        auto *args = static_cast<struct fstat_args *>(uap);

        FileContext &fdp = p.get_file_context();
        File *fp = nullptr;

        int error = getvnode(fdp, args->fd, fp);
        if (error != 0) {
            return error;
        }

        if (fp->f_data == nullptr) {
            return EINVAL;
        }

        auto *vp = static_cast<VNode *>(fp->f_data);

        struct stat sb;
        error = vp->vn_stat(&sb, p);
        if (error != 0) {
            return error;
        }

        memmove(args->sb, &sb, sizeof(sb));
        return 0;
    }

    int sys_fstatfs(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct fstatfs_args {
            int fd;
            struct statfs *buf;
        };
        auto *args = static_cast<struct fstatfs_args *>(uap);
        File *fp = nullptr;
        int error = getvnode(p.get_file_context(), args->fd, fp);

        if (error != 0)
            return error;

        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr || vp->v_mount == nullptr)
            return EINVAL;

        struct statfs sb;

        error = VFS_STATFS(vp->v_mount, &sb, p);
        if (error != 0)
            return error;

        sb.f_flags = vp->v_mount->mnt_flag & MNT_VISFLAGMASK;
        memmove(args->buf, &sb, sizeof(sb));
        return 0;
    }

    int sys_fchdir(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct fchdir_args {
            int fd;
        };
        auto *args = static_cast<struct fchdir_args *>(uap);
        File *fp = nullptr;

        int error = getvnode(p.get_file_context(), args->fd, fp);
        if (error != 0)
            return error;

        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr)
            return EINVAL;

        error = vp->ops->access(*vp, VEXEC, p.get_user_credentials(), p);
        if (error != 0)
            return error;
        if (vp->v_type != VDIR)
            return ENOTDIR;

        FileContext &fdp = p.get_file_context();
        VREF(vp);
        if (fdp.fd_cdir != nullptr)
            vrele(fdp.fd_cdir);
        fdp.fd_cdir = vp;
        return 0;
    }

    int sys_fchmod(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct fchmod_args {
            int fd;
            int fmode;
        };
        auto *args = static_cast<struct fchmod_args *>(uap);
        File *fp = nullptr;
        int error = getvnode(p.get_file_context(), args->fd, fp);
        if (error != 0)
            return error;

        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr || vp->v_mount == nullptr)
            return EINVAL;
        if (vp->v_mount->mnt_flag & MNT_RDONLY)
            return EROFS;

        VAttr vap;
        vattr_null(&vap);
        vap.va_mode = static_cast<mode_t>(args->fmode & 07777);
        vp->ops->lock(*vp);
        error = vp->ops->setattr(*vp, vap, p.get_user_credentials(), p);
        vp->ops->unlock(*vp);
        return error;
    }

    int sys_fchown(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct fchown_args {
            int fd;
            int uid;
            int gid;
        };
        auto *args = static_cast<struct fchown_args *>(uap);
        File *fp = nullptr;
        int error = getvnode(p.get_file_context(), args->fd, fp);
        if (error != 0)
            return error;

        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr || vp->v_mount == nullptr)
            return EINVAL;
        if (vp->v_mount->mnt_flag & MNT_RDONLY)
            return EROFS;

        VAttr vap;
        vattr_null(&vap);
        vap.va_uid = args->uid;
        vap.va_gid = args->gid;
        vp->ops->lock(*vp);
        error = vp->ops->setattr(*vp, vap, p.get_user_credentials(), p);
        vp->ops->unlock(*vp);
        return error;
    }

    // TODO: implement
    int sys_utimes(Sys::Process &p, void *uap, int *retval) {
        (void) p;
        (void) uap;
        (void) retval;
        return ENOSYS;
    }

    int sys_ftruncate(Sys::Process &p, void *uap, int *retval) {
        (void) retval;
        struct ftruncate_args {
            int fd;
            off_t length;
        };

        auto *args = static_cast<struct ftruncate_args *>(uap);
        if (args->length < 0)
            return EINVAL;
        File *fp = nullptr;
        int error = getvnode(p.get_file_context(), args->fd, fp);
        if (error != 0)
            return error;

        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr)
            return EINVAL;
        if ((fp->f_flag & FWRITE) == 0)
            return EBADF;
        if (vp->v_type == VDIR)
            return EISDIR;
        if ((error = vp->writechk()) != 0)
            return error;
        if ((error = vp->ops->access(*vp, VWRITE, p.get_user_credentials(), p)) != 0)
            return error;

        VAttr vap;
        vattr_null(&vap);
        vap.va_qsize = static_cast<unsigned long long>(args->length);
        vp->ops->lock(*vp);
        error = vp->ops->setattr(*vp, vap, p.get_user_credentials(), p);
        vp->ops->unlock(*vp);
        return error;
    }

    /*
     * Read a block of directory entries in a file system independent format.
     */
    int sys_getdirentries(Sys::Process &p, void *uap, int *retval) {
        struct getdirentries_args {
            int fd;
            char *buf;
            unsigned int count;
            long *basep;
        };
        auto *args = static_cast<struct getdirentries_args *>(uap);

        FileContext &fdp = p.get_file_context();
        File *fp = nullptr;
        int error = getvnode(fdp, args->fd, fp);
        if (error != 0) {
            return error;
        }

        if ((fp->f_flag & FREAD) == 0) {
            return EBADF;
        }
        auto *vp = static_cast<VNode *>(fp->f_data);
        if (vp == nullptr) {
            return EINVAL;
        }

        // getdirentries only for dirs
        if (vp->v_type != VDIR) {
            return EINVAL;
        }

        struct iovec aiov = {args->buf, static_cast<int>(args->count)};
        struct uio auio = {
                &aiov,
                1,
                static_cast<off_t>(fp->f_offset),
                static_cast<int>(args->count),
                UIO_SYSSPACE,
                UIO_READ,
                p};

        vp->ops->lock(*vp);
        const off_t off = fp->f_offset;
        int eofflag = 0;
        error = vp->ops->readdir(*vp, &auio, fp->f_cred, &eofflag);
        fp->f_offset = auio.uio_offset;

        vp->ops->unlock(*vp);

        if (error != 0) {
            return error;
        }
        if (args->basep != nullptr) {
            memmove(args->basep, &off, sizeof(long));
        }

        *retval = static_cast<int>(args->count) - auio.uio_resid;
        return 0;
    }

    // TODO: IMPLEMENT
    int sys_revoke(Sys::Process &p, void *uap, int *retval) {
        (void) p;
        (void) uap;
        (void) retval;
        return ENOSYS;
    }

} // namespace VFS
