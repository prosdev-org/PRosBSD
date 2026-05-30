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
 *	@(#)vfs_vnode.hxx
 */

#ifndef VFS_VNODE_HXX
#define VFS_VNODE_HXX

#include <sys/process.hxx>
#include <vfs/vfs_vnops.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <sys/types.h>
#include <vfs/vnode.h>
#include <vfs/mount.h>
#include <sys/uio.hxx>

namespace VFS {
    class VNode {
    public:
        unsigned long v_flag; /* vnode flags (see below) */
        short v_usecount; /* reference count of users */
        short v_writecount; /* reference count of writers */
        long v_holdcnt; /* page & buffer references */
        off_t v_lastr; /* last read (read-ahead) */
        unsigned long v_id; /* capability identifier */
        struct mount *v_mount; /* ptr to vfs we are in */
        VNodeOps *ops; /* vnode operations */
        VNode* v_freef; /* vnode freelist forward */
        VNode* v_freeb; /* vnode freelist back */
        VNode* v_mountf; /* vnode mountlist forward */
        VNode* v_mountb; /* vnode mountlist back */
        struct buf *v_cleanblkhd; /* clean blocklist head */
        struct buf *v_dirtyblkhd; /* dirty blocklist head */
        long v_numoutput; /* num of writes in progress */
        enum vtype v_type; /* vnode type */
        enum vtagtype v_tag; /* type of underlying data */
        char v_data[VN_MAXPRIVATE]; /* private data for fs */

        int open(VNode *&vp, Nameidata &ndp, Sys::Process &p, int fmode, int cmode);
        [[nodiscard]] int writechk() const;
        int close(int flags, Sys::UserCredentials &cred, Sys::Process &p);
        int rdwr(enum uio_rw rw, void *base, int len, off_t offset, enum uio_seg segflg, int ioflg, Sys::UserCredentials &cred, int *aresid, Sys::Process &p);
        int read(struct file *fp, struct uio *uio, Sys::UserCredentials &cred);
        int write(struct file *fp, struct uio *uio, Sys::UserCredentials &cred);
        int stat(struct stat *sb, Sys::Process &p);
        int ioctl(struct file *fp, unsigned int com, void* data, Sys::Process &p);
        int select(struct file *fp, int which, Sys::Process &p);
        int closefile(struct file *fp, Sys::Process &p);
        int fhtovp(fhandle_t *fhp, int lockflag);
    };
}

#endif // VFS_VNODE_HXX