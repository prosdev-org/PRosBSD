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

#ifndef VFS_VFSOPS_HXX
#define VFS_VFSOPS_HXX

#include <vfs/mount.hxx>

namespace Sys {
    class Process;
}

namespace VFS {

    class VNode;
    class Nameidata;

    class VFSOps {
    public:
        virtual ~VFSOps() = default;


        virtual int mount(Mount &mp, char *path, void *data,
                          Nameidata &ndp, Sys::Process &p) = 0;
        virtual int start(Mount &mp, int flags, Sys::Process &p) = 0;
        virtual int unmount(Mount &mp, int mntflags, Sys::Process &p) = 0;
        virtual int root(Mount &mp, VNode *&vpp) = 0;
        virtual int statfs(Mount &mp, struct statfs &sbp, Sys::Process &p) = 0;
        virtual int sync(Mount &mp, int waitfor) = 0;
        virtual int fhtovp(Mount &mp, struct fid *fhp, VNode *&vpp) = 0;
        virtual int vptofh(VNode &vp, struct fid *fhp) = 0;
        virtual int init() = 0; // At boot
    };

#define VFS_MOUNT(MP, PATH, DATA, NDP, P) \
    ((MP)->mnt_op->mount(*(MP), (PATH), (DATA), *(NDP), (P)))
#define VFS_START(MP, FLAGS, P) \
    ((MP)->mnt_op->start(*(MP), (FLAGS), (P)))
#define VFS_UNMOUNT(MP, FORCE, P) \
    ((MP)->mnt_op->unmount(*(MP), (FORCE), (P)))
#define VFS_ROOT(MP, VPP) \
    ((MP)->mnt_op->root(*(MP), *(VPP)))
#define VFS_STATFS(MP, SBP, P) \
    ((MP)->mnt_op->statfs(*(MP), *(SBP), (P)))
#define VFS_SYNC(MP, WAITFOR) \
    ((MP)->mnt_op->sync(*(MP), (WAITFOR)))
#define VFS_FHTOVP(MP, FIDP, VPP) \
    ((MP)->mnt_op->fhtovp(*(MP), (FIDP), *(VPP)))
#define VFS_VPTOFH(VP, FIDP) \
    ((VP)->v_mount->mnt_op->vptofh(*(VP), (FIDP)))

} // namespace VFS

#endif // VFS_VFSOPS_HXX
