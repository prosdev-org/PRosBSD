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
#include <machine/cpu.hxx>
#include <string.h>
#include <sys/kernel.hxx>
#include <sys/panic.hxx>
#include <unique/log.hxx>
#include <vfs/mount.hxx>
#include <vfs/name_cache.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vattr.hxx>
#include <vfs/vfs_vnode.hxx>
#include <vfs/vfsops.hxx>

namespace VFS {

    using Sys::Kernel::proc0_ref;

    static kxx::StringView get_logger_prefix() {
        return "vfs/subr";
    }

    Mount *rootfs = nullptr;
    VNode *rootdir = nullptr;
    long desiredvnodes = 256;
    long numvnodes = 0;
    VAttr va_null;
    VFSOps *vfssw[MOUNT_MAXTYPE + 1] = {};
    static VNode *vfreeh = nullptr;
    static VNode **vfreet = &vfreeh;
    extern VNodeOps &dead_vnodeops;

    void vfs_init() {
        LOG("initializing VFS");

        /*
         * Initialize the vnode name cache
         */
        NameCache::init();
        vattr_null(&va_null);

        /*
         * Initialize each file system type.
         */
        for (int i = 0; i <= MOUNT_MAXTYPE; i++) {
            if (vfssw[i] != nullptr) {
                vfssw[i]->init();
            }
        }

        // TODO: RAMFS AS ROOT!
        if (rootfs == nullptr && vfssw[MOUNT_RAMFS] != nullptr) {
            Sys::Process &p = *Sys::Kernel::proc0_ref();

            const int error = vfs_mountroot(p);
            if (error != 0) {
                Sys::panic("vfs_init: cannot mount rootfs ", error);
            }
        }

        LOG("VFS initialized");
    }

    /*
     * Set vnode attributes to VNOVAL.
     */
    void vattr_null(VAttr *vap) {
        vap->va_type = VNON;

        vap->va_mode = static_cast<mode_t>(VNOVAL);
        vap->va_nlink = static_cast<nlink_t>(VNOVAL);
        vap->va_uid = static_cast<uid_t>(VNOVAL);
        vap->va_gid = static_cast<gid_t>(VNOVAL);

        vap->va_fsid = static_cast<long>(VNOVAL);
        vap->va_fileid = static_cast<long>(VNOVAL);

        vap->va_qsize = static_cast<unsigned long long>(VNOVAL);
        vap->va_blocksize = static_cast<long>(VNOVAL);

        vap->va_atime.tv_sec = static_cast<time_t>(VNOVAL);
        vap->va_atime.tv_usec = static_cast<suseconds_t>(VNOVAL);

        vap->va_mtime.tv_sec = static_cast<time_t>(VNOVAL);
        vap->va_mtime.tv_usec = static_cast<suseconds_t>(VNOVAL);

        vap->va_ctime.tv_sec = static_cast<time_t>(VNOVAL);
        vap->va_ctime.tv_usec = static_cast<suseconds_t>(VNOVAL);

        vap->va_gen = static_cast<unsigned long>(VNOVAL);
        vap->va_flags = static_cast<unsigned long>(VNOVAL);
        vap->va_rdev = static_cast<dev_t>(VNOVAL);
        vap->va_qbytes = static_cast<unsigned long long>(VNOVAL);
    }


    /*
     * Remove a mount point from the list of mounted filesystems.
     * Unmount of the root is illegal.
     */
    void vfs_remove(Mount *mp) {
        if (mp == rootfs) {
            Sys::panic("vfs_remove: unmounting root");
        }
        mp->mnt_prev->mnt_next = mp->mnt_next;
        mp->mnt_next->mnt_prev = mp->mnt_prev;
        mp->mnt_vnodecovered->v_mount = nullptr;
    }

    // TODO: implement locks!
    /*
     * Lock a filesystem.
     * Used to prevent access to it while mounting and unmounting.
     */
    int vfs_lock(Mount *mp) {
        while (mp->mnt_flag & MNT_MLOCK) {
            mp->mnt_flag |= MNT_MWAIT;
            Machine::Cpu::relax(); // dirty hack
        }
        mp->mnt_flag |= MNT_MLOCK;
        return 0;
    }

    /*
     * Unlock a locked filesystem.
     * Panic if filesystem is not locked.
     */
    void vfs_unlock(Mount *mp) {
        if ((mp->mnt_flag & MNT_MLOCK) == 0) {
            Sys::panic("vfs_unlock: not locked");
        }
        mp->mnt_flag &= ~MNT_MLOCK;
        if (mp->mnt_flag & MNT_MWAIT) {
            mp->mnt_flag &= ~MNT_MWAIT;
            // sorry Michael Karels, Kirk McKusick
            // even for your code there is NO actual locking.
        }
    }

    /*
     * Mark a mount point as busy.
     * Used to synchronize access and to delay unmounting.
     */
    int vfs_busy(Mount *mp) {
        while (mp->mnt_flag & MNT_MPBUSY) {
            mp->mnt_flag |= MNT_MPWANT;
            Machine::Cpu::relax();
        }
        if (mp->mnt_flag & MNT_UNMOUNT) {
            return 1;
        }
        mp->mnt_flag |= MNT_MPBUSY;
        return 0;
    }

    /*
     * Free a busy filesystem.
     * Panic if filesystem is not busy.
     */
    int vfs_unbusy(Mount *mp) {
        if ((mp->mnt_flag & MNT_MPBUSY) == 0) {
            Sys::panic("vfs_unbusy: not busy");
        }
        mp->mnt_flag &= ~MNT_MPBUSY;
        if (mp->mnt_flag & MNT_MPWANT) {
            mp->mnt_flag &= ~MNT_MPWANT;
            // no wakeup
        }
        return 0;
    }

    // register vfs
    void vfs_register(const int type, VFSOps *ops) {
        ASSERT(type >= 0);
        ASSERT(type <= MOUNT_MAXTYPE);
        ASSERT(ops != nullptr);

        vfssw[type] = ops;
    }

    // root mount
    int vfs_mountroot(Sys::Process &p) {
        if (rootfs != nullptr) {
            return EBUSY;
        }

        VFSOps *ops = vfssw[MOUNT_RAMFS];
        if (ops == nullptr) {
            return ENODEV;
        }

        auto *mp = new Mount();
        mp->mnt_op = ops;
        mp->mnt_flag = 0;

        vfs_lock(mp);

        static char rootpath[] = "/";

        Nameidata nd;
        nd.ni_dirp = rootpath;
        nd.ni_segflg = UIO_SYSSPACE;

        int error = VFS_MOUNT(mp, rootpath, nullptr, &nd, p);
        if (error != 0) {
            vfs_unlock(mp);
            delete mp;
            return error;
        }

        error = VFS_START(mp, 0, p);
        if (error != 0) {
            vfs_unlock(mp);
            delete mp;
            return error;
        }

        VNode *vp = nullptr;
        error = VFS_ROOT(mp, &vp);
        if (error != 0) {
            vfs_unlock(mp);
            delete mp;
            return error;
        }

        // root fs is active now
        mp->mnt_next = mp;
        mp->mnt_prev = mp;
        rootfs = mp;

        vp->v_flag |= VROOT;
        rootdir = vp;

        // TODO: FIX DIRTY HACK WITH proc0!!!!!!!!!!!!!!!!!!!!!
        FileContext &fdp = p.get_file_context();

        if (fdp.fd_rdir != nullptr) {
            vrele(fdp.fd_rdir);
        }

        if (fdp.fd_cdir != nullptr) {
            vrele(fdp.fd_cdir);
        }

        fdp.fd_rdir = vp;
        fdp.fd_cdir = vp;

        VREF(vp); // fd_rdir
        VREF(vp); // fd_cdir

        vfs_unlock(mp);

        LOG("root fs mounted");
        return 0;
    }

    /*
     * Lookup a mount point by filesystem identifier.
     */
    Mount *getvfs(fsid_t *fsid) {
        Mount *mp = rootfs;
        if (mp == nullptr) {
            return nullptr;
        }
        do {
            if (mp->mnt_stat.f_fsid == *fsid) {
                return mp;
            }
            mp = mp->mnt_next;
        } while (mp != rootfs);
        return nullptr;
    }

    /*
     * Return the next vnode from the free list.
     */
    int getnewvnode(enum vtagtype tag, Mount *mp, VNodeOps *vops, VNode *&vpp) {
        VNode *vp;
        VNode *vq;

        if (numvnodes < desiredvnodes) {
            vp = new VNode();
            numvnodes++;
        } else {
            if ((vp = vfreeh) == nullptr) {
                vpp = nullptr;
                return ENFILE;
            }
            if (vp->v_usecount) {
                Sys::panic("getnewvnode: free vnode isn't");
            }
            if ((vq = vp->v_freef) != nullptr) {
                vq->v_freeb = &vfreeh;
            } else {
                vfreet = &vfreeh;
            }
            vfreeh = vq;
            vp->v_freef = nullptr;
            vp->v_freeb = nullptr;
            if (vp->v_type != VBAD) {
                vgone(vp);
            }
            vp->v_flag = 0;
            vp->v_lastr = 0;
        }

        vp->v_type = VNON;
        NameCache::purge(*vp);
        vp->v_tag = tag;
        vp->ops = vops;
        insmntque(vp, mp);
        VREF(vp);
        vpp = vp;
        return 0;
    }

    /*
     * Move a vnode from one mount queue to another.
     */
    void insmntque(VNode *vp, Mount *mp) {
        VNode *vq;

        /* Delete from old mount point vnode list, if on one. */
        if (vp->v_mountb != nullptr) {
            if ((vq = vp->v_mountf) != nullptr) {
                vq->v_mountb = vp->v_mountb;
            }
            *vp->v_mountb = vq;
        }

        /*
         * Insert into list of vnodes for the new mount point, if available.
         */
        vp->v_mount = mp;
        if (mp == nullptr) {
            vp->v_mountf = nullptr;
            vp->v_mountb = nullptr;
            return;
        }
        if ((vq = mp->mnt_mounth) != nullptr) {
            vq->v_mountb = &vp->v_mountf;
        }
        vp->v_mountf = vq;
        vp->v_mountb = &mp->mnt_mounth;
        mp->mnt_mounth = vp;
    }

    /*
     * Flush all dirty buffers associated with a vnode.
     */
    void vflushbuf(VNode *vp, int flags) {
        // TODO: IMPLEMENT
        (void) vp;
        (void) flags;
    }

    int vflush(Mount *mp, VNode *skipvp, int flags) {
        VNode *vp;
        VNode *nvp;
        int busy = 0;

        if ((mp->mnt_flag & MNT_MPBUSY) == 0) {
            Sys::panic("vflush: not busy");
        }

    loop:
        for (vp = mp->mnt_mounth; vp != nullptr; vp = nvp) {
            if (vp->v_mount != mp) {
                goto loop;
            }
            nvp = vp->v_mountf;

            if (vp == skipvp) {
                continue;
            }
            if ((flags & SKIPSYSTEM) && (vp->v_flag & VSYSTEM)) {
                continue;
            }

            if (vp->v_usecount == 0) {
                vgone(vp);
                continue;
            }

            // kill all nodes
            if (flags & FORCECLOSE) {
                if (vp->v_type != VBLK && vp->v_type != VCHR) {
                    vgone(vp);
                } else {
                    vclean(vp, 0);
                    insmntque(vp, nullptr);
                }
                continue;
            }
            busy++;
        }

        if (busy) {
            return EBUSY;
        }
        return 0;
    }

    /*
     * Grab a particular vnode from the free list, increment its
     * reference count and lock it. The vnode lock bit is set the
     * vnode is being eliminated in vgone. The process is awakened
     * when the transition is completed, and an error returned to
     * indicate that the vnode is no longer usable (possibly having
     * been changed to a new file system type).
     */
    void vget(VNode *vp) {
        VNode *vq;

        while (vp->v_flag & VXLOCK) {
            vp->v_flag |= VXWANT;
            Machine::Cpu::relax();
        }
        if (vp->v_usecount == 0) {
            if ((vq = vp->v_freef) != nullptr) {
                vq->v_freeb = vp->v_freeb;
            } else {
                vfreet = vp->v_freeb;
            }
            *vp->v_freeb = vq;
            vp->v_freef = nullptr;
            vp->v_freeb = nullptr;
        }
        VREF(vp);
        if (vp->ops != nullptr) {
            vp->ops->lock(*vp);
        }
    }

    /*
     * Vnode reference, just increment the count.
     */
    void vref(VNode *vp) {
        vp->v_usecount++;
    }

    /*
     * vput(), just unlock and vrele().
     */
    void vput(VNode *vp) {
        if (vp->ops != nullptr) {
            vp->ops->unlock(*vp);
        }
        vrele(vp);
    }

    /*
     * Vnode release.
     * If count drops to zero, call inactive routine and return to freelist.
     */
    void vrele(VNode *vp) {
        if (vp == nullptr) {
            Sys::panic("vrele: null vp");
        }
        if (vp->v_usecount <= 0) {
            Sys::panic("vrele: zero usecount");
        }
        vp->v_usecount--;
        if (vp->v_usecount > 0) {
            return;
        }
        if (vp->v_usecount != 0 || vp->v_writecount != 0) {
            Sys::panic("vrele: ref cnt");
        }

        /* Insert at tail of free list */
        if (vfreeh == nullptr) {
            vfreeh = vp;
            vp->v_freeb = &vfreeh;
        } else {
            *vfreet = vp;
            vp->v_freeb = vfreet;
        }
        vp->v_freef = nullptr;
        vfreet = &vp->v_freef;

        if (vp->ops != nullptr) {
            vp->ops->inactive(*vp, *proc0_ref());
        }
    }

    /*
     * Disassociate the underlying file system from a vnode.
     */
    void vclean(VNode *vp, int flags) {
        VNodeOps *origops;
        int active;

        if ((active = vp->v_usecount) != 0) {
            VREF(vp);
        }

        if (vp->v_flag & VXLOCK) { // that would be funny
            Sys::panic("vclean: deadlock");
        }
        vp->v_flag |= VXLOCK;

        if (vp->ops != nullptr) {
            vp->ops->lock(*vp);
        }

        // no operations further
        origops = vp->ops;
        vp->ops = &dead_vnodeops;
        vp->v_tag = VT_NON;

        if (origops != nullptr) {
            origops->unlock(*vp);
            if (active) {
                if (flags & DOCLOSE) {
                    origops->close(*vp, IO_NDELAY, proc0_ref()->get_user_credentials(), *proc0_ref());
                }
                origops->inactive(*vp, *proc0_ref());
            }
            if (origops->reclaim(*vp) != 0) {
                Sys::panic("vclean: cannot reclaim");
            }
        }

        if (active) {
            vrele(vp);
        }

        vp->v_flag &= ~VXLOCK;
        if (vp->v_flag & VXWANT) {
            vp->v_flag &= ~VXWANT;
            // TODO: wakeup
        }
    }

    /*
     * Eliminate all activity associated with a vnode
     * in preparation for reuse.
     */
    void vgone(VNode *vp) {
        while (vp->v_flag & VXLOCK) {
            vp->v_flag |= VXWANT;
            Machine::Cpu::relax();
        }

        vclean(vp, DOCLOSE);

        // delete from mountpoint
        if (vp->v_mountb != nullptr) {
            VNode *vq;
            if ((vq = vp->v_mountf) != nullptr) {
                vq->v_mountb = vp->v_mountb;
            }
            *vp->v_mountb = vq;
            vp->v_mountf = nullptr;
            vp->v_mountb = nullptr;
        }

        // move to free head
        if (vp->v_freeb != nullptr) {
            VNode *vq;
            if ((vq = vp->v_freef) != nullptr) {
                vq->v_freeb = vp->v_freeb;
            } else {
                vfreet = vp->v_freeb;
            }
            *vp->v_freeb = vq;
            vp->v_freef = vfreeh;
            vp->v_freeb = &vfreeh;
            if (vfreeh != nullptr) {
                vfreeh->v_freeb = &vp->v_freef;
            }
            vfreeh = vp;
        }

        vp->v_type = VBAD;
    }

    /*
     * Eliminate all activity associated with the requested vnode
     * and with all vnodes aliased to the requested vnode.
     */
    void vgoneall(VNode *vp) {
        if (vp->v_flag & VALIASED) {
            if (vp->v_flag & VXLOCK) {
                vp->v_flag |= VXWANT;
                Machine::Cpu::relax();
                return;
            }
            vp->v_flag |= VXLOCK;
            vp->v_flag &= ~VALIASED;
        }
        vgone(vp);
    }

    int vfinddev(dev_t dev, enum vtype type, VNode *&vvp) {
        // TODO: IMPLEMENT
        (void) dev;
        (void) type;
        (void) vvp;
        return 1;
    }

    /*
     * Calculate the total number of references to a special device.
     */
    int vcount(VNode *vp) {
        return vp->v_usecount;
    }

    // TODO: IMPLEMENT
    void bgetvp(VNode *vp, void *bp) {
        (void) vp;
        (void) bp;
    }
    void brelvp(void *bp) {
        (void) bp;
    }
    void reassignbuf(void *bp, VNode *newvp) {
        (void) bp;
        (void) newvp;
    }
    void vwakeup(void *bp) {
        (void) bp;
    }

    /*
     * Print out a description of a vnode.
     */
    static const char *typename_str[] = {
            "VNON", "VREG", "VDIR", "VBLK", "VCHR",
            "VLNK", "VSOCK", "VFIFO", "VBAD"};

    void vprint(const char *label, VNode *vp) {
        if (label != nullptr) {
            LOG(label, ": ");
        }
        LOG("type ", typename_str[vp->v_type],
            ", usecount ", vp->v_usecount,
            ", writecount ", vp->v_writecount,
            ", refcount ", vp->v_holdcnt);
    }

    void printlockedvnodes() {
        Mount *mp = rootfs;
        if (mp == nullptr) {
            return;
        }
        LOG("Locked vnodes:");
        do {
            for (VNode *vp = mp->mnt_mounth; vp != nullptr; vp = vp->v_mountf) {
                if (vp->v_flag & VXLOCK) {
                    vprint(nullptr, vp);
                }
            }
            mp = mp->mnt_next;
        } while (mp != rootfs);
    }

} // namespace VFS
