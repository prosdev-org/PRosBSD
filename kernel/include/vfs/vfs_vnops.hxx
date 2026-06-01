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
 *	@(#)vfs_vnops.hxx
 */

#ifndef VFS_VNOPS_HXX
#define VFS_VNOPS_HXX

#include <sys/process.hxx>
#include <sys/types.h>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_vattr.hxx>

namespace VFS {
    // So much stubs..
    class BUF {
    };

    class FLock {
    };
    // TODO: Fill the stubs

    class VNode;
    class VNodeOps { // add consts, when needed
    public:
        virtual ~VNodeOps() = default;

        virtual int lookup(VNode &vp, Nameidata &ndp, Sys::Process &p) = 0;
        virtual int create(Nameidata &ndp, VAttr &vap, Sys::Process &p) = 0;
        virtual int mknod(Nameidata &ndp, VAttr &vap, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int open(VNode &vp, int mode, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int close(VNode &vp, int fflag, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int acess(VNode &vp, int mode, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int getattr(VNode &vp, VAttr &vap, Sys::UserCredentials &cred) = 0;
        virtual int setattr(VNode &vp, VAttr &vap, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int read(VNode &vp, struct uio *uio, int ioflag, Sys::UserCredentials &cred) = 0;
        virtual int write(VNode vp, struct uio *uio, int ioflag, Sys::UserCredentials &cred) = 0;
        virtual int ioctl(VNode &vp, unsigned int command, void *data, int fflag, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int select(VNode &vp, int which, int fflag, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int mmap(VNode &vp, int fflags, Sys::UserCredentials &cred, Sys::Process &p) = 0;
        virtual int fsync(VNode &vp, int fflags, Sys::UserCredentials &cred, int waitfor, Sys::Process &p) = 0;
        virtual int seek(VNode &vp, off_t oldoff, off_t newoff, Sys::UserCredentials &cred) = 0;
        virtual int remove(Nameidata &ndp, Sys::Process &p) = 0;
        virtual int link(VNode &vp, Nameidata &ndp, Sys::Process &p) = 0;
        virtual int rename(Nameidata &fndp, Nameidata &tndp, Sys::Process &p) = 0;
        virtual int mkdir(Nameidata &ndp, VAttr &vap, Sys::Process &p) = 0;
        virtual int rmdir(Nameidata &ndp, Sys::Process &p) = 0;
        virtual int symlink(Nameidata &ndp, VAttr &vap, char *target, Sys::Process &p) = 0;
        virtual int readdir(VNode &vp, struct uio *uio, Sys::UserCredentials &cred, int *eofflagp) = 0;
        virtual int readlink(VNode &vp, struct uio *uio, Sys::UserCredentials &cred) = 0;
        virtual int aborttop(Nameidata &ndp) = 0;
        virtual int inactive(VNode &vp, Sys::Process &p) = 0;
        virtual int reclaim(VNode &vp) = 0;
        virtual int lock(VNode &vp) = 0;
        virtual int unlock(VNode &vp) = 0;
        virtual int bmap(VNode &vp, daddr_t bn, VNode &vpp, daddr_t *bnp) = 0;
        virtual int strategy(BUF &bp) = 0;
        virtual int print(VNode &vp) = 0;
        virtual int islocked(VNode &vp) = 0;
        virtual int advlock(VNode &vp, void *id, int op, FLock &fl, int flags) = 0;
    };
} // namespace VFS

#endif // VFS_VNOPS_HXX
