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
#include <libkxx/heap.hxx>
#include <machine/cpu.hxx>
#include <string.h>
#include <sys/panic.hxx>
#include <sys/process.hxx>
#include <unique/log.hxx>
#include <vfs/name_cache.hxx>
#include <vfs/vfs_lookup.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vnode.hxx>

namespace VFS {

    Nameidata::~Nameidata() {
        if (ni_pnbuf_owned && ni_pnbuf != nullptr) {
            kxx::Heap::free(ni_pnbuf);
            ni_pnbuf = nullptr;
            ni_pnbuf_owned = false;
        }
    }

    int namei(Nameidata &ndp, Sys::Process &p) {
        char *cp;
        VNode *dp;
        int error;
        int linklen;

        ndp.ni_cred = p.get_user_credentials();

        if ((ndp.ni_nameiop & HASBUF) == 0) {
            ndp.ni_pnbuf = static_cast<char *>(
                    kxx::Heap::malloc(MAXPATHLEN + 1));
            ndp.ni_pnbuf_owned = true;
            if (ndp.ni_pnbuf == nullptr) {
                ndp.ni_pnbuf_owned = false;
                return ENOMEM;
            }
        }

        // copy pathname
        // MAXPATHLEN is maximum
        const char *src = static_cast<const char *>(ndp.ni_dirp);
        if (src == nullptr) {
            return EFAULT;
        }
        size_t len = strnlen(src, MAXPATHLEN);
        if (len >= MAXPATHLEN) {
            return ENAMETOOLONG;
        }
        memmove(ndp.ni_pnbuf, src, len);
        ndp.ni_pnbuf[len] = '\0';
        ndp.ni_pathlen = static_cast<long>(len);

        ndp.ni_loopcnt = 0;

        // starting point
        FileContext &fdp = p.get_file_context();
        if ((ndp.ni_rootdir = fdp.fd_rdir) == nullptr) {
            ndp.ni_rootdir = rootdir;
        }
        dp = fdp.fd_cdir;
        VREF(dp);

        for (;;) {
            // if root should replace current
            ndp.ni_ptr = ndp.ni_pnbuf;
            if (*ndp.ni_ptr == '/') {
                vrele(dp);
                while (*ndp.ni_ptr == '/') {
                    ndp.ni_ptr++;
                    ndp.ni_pathlen--;
                }
                dp = ndp.ni_rootdir;
                VREF(dp);
            }
            ndp.ni_startdir = dp;
            if ((error = lookup(ndp, p)) != 0) {
                return error;
            }

            // symlinks!
            if (ndp.ni_more == 0) {
                if ((ndp.ni_nameiop & (SAVENAME | SAVESTART)) != 0) {
                    ndp.ni_nameiop |= HASBUF;
                }
                return 0;
            }

            if ((ndp.ni_nameiop & LOCKPARENT) && ndp.ni_pathlen == 1) {
                ndp.ni_dvp->ops->unlock(*ndp.ni_dvp);
            }
            if (ndp.ni_loopcnt++ >= MAXSYMLINKS) {
                error = ELOOP;
                break;
            }

            // symlink target
            char linkbuf[MAXPATHLEN];
            struct iovec aiov = {linkbuf, MAXPATHLEN};
            struct uio auio = {
                    &aiov, 1, 0, MAXPATHLEN,
                    UIO_SYSSPACE, UIO_READ, p};

            if ((error = ndp.ni_vp->ops->readlink(
                         *ndp.ni_vp, &auio, ndp.ni_cred)) != 0) {
                break;
            }

            linklen = MAXPATHLEN - auio.uio_resid;
            if (linklen + ndp.ni_pathlen >= MAXPATHLEN) {
                error = ENAMETOOLONG;
                break;
            }

            if (ndp.ni_pathlen > 1) {
                cp = static_cast<char *>(kxx::Heap::malloc(MAXPATHLEN));
                if (cp == nullptr) {
                    error = ENOMEM;
                    break;
                }
                memmove(cp, linkbuf, linklen);
                memmove(cp + linklen, ndp.ni_next, ndp.ni_pathlen);
                kxx::Heap::free(ndp.ni_pnbuf);
                ndp.ni_pnbuf = cp;
            } else {
                memmove(ndp.ni_pnbuf, linkbuf, linklen);
                ndp.ni_pnbuf[linklen] = '\0';
            }
            ndp.ni_pathlen += linklen;

            vput(ndp.ni_vp);
            dp = ndp.ni_dvp;
        }

        vrele(ndp.ni_dvp);
        vput(ndp.ni_vp);
        ndp.ni_vp = nullptr;
        return error;
    }

    // actual search
    int lookup(Nameidata &ndp, Sys::Process &p) {
        char *cp;
        VNode *dp = nullptr;
        VNode *tdp;
        Mount *mp;
        int docache;
        int flag;
        int wantparent;
        int rdonly;
        int error = 0;

        flag = ndp.ni_nameiop & OPMASK;
        wantparent = ndp.ni_nameiop & (LOCKPARENT | WANTPARENT);
        docache = (ndp.ni_nameiop & NOCACHE) ^ NOCACHE;
        if (flag == DELETE || (wantparent && flag != CREATE)) {
            docache = 0;
        }
        rdonly = MNT_RDONLY;
        if (ndp.ni_nameiop & REMOTE) {
            rdonly |= MNT_EXRDONLY;
        }

        ndp.ni_dvp = nullptr;
        ndp.ni_more = 0;
        dp = ndp.ni_startdir;
        ndp.ni_startdir = nullptr;
        dp->ops->lock(*dp);

    dirloop:
        // search for new dirs and compute hash for them
        ndp.ni_vp = nullptr;
        ndp.ni_hash = 0;
        for (cp = ndp.ni_ptr; *cp != '\0' && *cp != '/'; cp++) {
            ndp.ni_hash += static_cast<unsigned char>(*cp);
        }
        ndp.ni_namelen = cp - ndp.ni_ptr;

        if (ndp.ni_namelen >= MAXPATHLEN) {
            error = ENAMETOOLONG;
            goto bad;
        }

        ndp.ni_pathlen -= ndp.ni_namelen;
        ndp.ni_next = cp;
        ndp.ni_makeentry = 1;
        if (*cp == '\0' && docache == 0) {
            ndp.ni_makeentry = 0;
        }
        ndp.ni_isdotdot = (ndp.ni_namelen == 2 &&
                           ndp.ni_ptr[1] == '.' &&
                           ndp.ni_ptr[0] == '.');

        // check for illegal
        if (ndp.ni_ptr[0] == '\0') {
            if (flag != LOOKUP || wantparent) {
                error = EISDIR;
                goto bad;
            }
            if (dp->v_type != VDIR) {
                error = ENOTDIR;
                goto bad;
            }
            if (!(ndp.ni_nameiop & LOCKLEAF)) {
                dp->ops->unlock(*dp);
            }
            ndp.ni_vp = dp;
            if (ndp.ni_nameiop & SAVESTART) {
                Sys::panic("lookup: SAVESTART");
            }
            return 0;
        }

        // ".." handler
        if (ndp.ni_isdotdot) {
            for (;;) {
                if (dp == ndp.ni_rootdir) {
                    ndp.ni_dvp = dp;
                    ndp.ni_vp = dp;
                    VREF(dp);
                    goto nextname;
                }
                if ((dp->v_flag & VROOT) == 0 ||
                    (ndp.ni_nameiop & NOCROSSMOUNT)) {
                    break;
                }
                tdp = dp;
                dp = dp->v_mount->mnt_vnodecovered;
                vput(tdp);
                VREF(dp);
                dp->ops->lock(*dp);
            }
        }

        // try
        ndp.ni_dvp = dp;
        if (docache && (error = NameCache::lookup(ndp)) != 0) {
            if (error == -1) {
                // found
                error = 0;
                dp = ndp.ni_vp;
                goto got_vp;
            }
            if (error == ENOENT && flag == LOOKUP) {
                // 404
                ndp.ni_vp = nullptr;
                if (wantparent) {
                    ndp.ni_dvp = dp;
                    if (!(ndp.ni_nameiop & LOCKPARENT)) {
                        dp->ops->unlock(*dp);
                    }
                    return ENOENT;
                }
                vrele(dp);
                return ENOENT;
            }
            error = 0;
        }

        // if we have access
        if ((error = dp->ops->access(*dp, VEXEC, ndp.ni_cred, p)) != 0) {
            goto bad;
        }

        // lookup from end FS
        if ((error = dp->ops->lookup(*dp, ndp, p)) != 0) {
            if (flag == LOOKUP || flag == DELETE ||
                error != ENOENT || *cp != '\0') {
                goto bad;
            }
            // if allowed, basically
            if (ndp.ni_dvp->v_mount->mnt_flag & rdonly) {
                error = EROFS;
                goto bad;
            }
            // entry is NULL
            if (ndp.ni_makeentry && docache) {
                NameCache::enter(ndp);
            }
            if (ndp.ni_nameiop & SAVESTART) {
                ndp.ni_startdir = ndp.ni_dvp;
                VREF(ndp.ni_startdir);
            }
            return 0;
        }

        dp = ndp.ni_vp;

    got_vp:
        // symlink
        if ((dp->v_type == VLNK) &&
            ((ndp.ni_nameiop & FOLLOW) || *ndp.ni_next == '/')) {
            ndp.ni_more = 1;
            return 0;
        }

    // find root
    mntloop:
        while (dp->v_type == VDIR &&
               (mp = dp->v_mount) != nullptr &&
               dp->v_flag & VROOT &&
               (ndp.ni_nameiop & NOCROSSMOUNT) == 0) {
            // crossmount
            Mount *mounted = nullptr;
            // find vnode's FS
            Mount *scan = rootfs;
            if (scan != nullptr) {
                do {
                    if (scan->mnt_vnodecovered == dp) {
                        mounted = scan;
                        break;
                    }
                    scan = scan->mnt_next;
                } while (scan != rootfs);
            }
            if (mounted == nullptr) {
                break;
            }
            while (mounted->mnt_flag & MNT_MLOCK) {
                mounted->mnt_flag |= MNT_MWAIT;
                Machine::Cpu::relax();
                goto mntloop;
            }
            VNode *newdp;
            if ((error = VFS_ROOT(mounted, &newdp)) != 0) {
                goto bad2;
            }
            vput(dp);
            ndp.ni_vp = dp = newdp;
        }

    nextname:
        // not a symlink
        if (*ndp.ni_next == '/') {
            ndp.ni_ptr = ndp.ni_next;
            while (*ndp.ni_ptr == '/') {
                ndp.ni_ptr++;
                ndp.ni_pathlen--;
            }
            vrele(ndp.ni_dvp);
            goto dirloop;
        }

        // if RDONLY
        if (flag == DELETE || flag == RENAME) {
            if ((dp->v_mount->mnt_flag & rdonly) ||
                (wantparent && (ndp.ni_dvp->v_mount->mnt_flag & rdonly))) {
                error = EROFS;
                goto bad2;
            }
        }

        // add to cache, if needed
        if (ndp.ni_makeentry && docache) {
            NameCache::enter(ndp);
        }

        if (ndp.ni_nameiop & SAVESTART) {
            ndp.ni_startdir = ndp.ni_dvp;
            VREF(ndp.ni_startdir);
        }
        if (!wantparent) {
            vrele(ndp.ni_dvp);
        }
        if ((ndp.ni_nameiop & LOCKLEAF) == 0) {
            dp->ops->unlock(*dp);
        }
        return 0;

    bad2:
        if ((ndp.ni_nameiop & LOCKPARENT) && *ndp.ni_next == '\0') {
            ndp.ni_dvp->ops->unlock(*ndp.ni_dvp);
        }
        vrele(ndp.ni_dvp);
    bad:
        vput(dp);
        ndp.ni_vp = nullptr;
        return error;
    }

} // namespace VFS
