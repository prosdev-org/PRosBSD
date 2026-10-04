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
 *	@(#)vfs_vnode.hxx
 */

#ifndef VFS_VNODE_HXX
#define VFS_VNODE_HXX

#include <sys/process.hxx>
#include <sys/types.h>
#include <sys/uio.hxx>
#include <vfs/mount.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_types.hxx>
#include <vfs/vfs_vattr.hxx>
#include <vfs/vfs_vnops.hxx>

namespace VFS {

    class VNode {
    public:
        unsigned long v_flag = 0;
        short v_usecount = 0;
        short v_writecount = 0;
        long v_holdcnt = 0;
        off_t v_lastr = 0;
        unsigned long v_id = 0;
        Mount *v_mount = nullptr;
        VNodeOps *ops = nullptr;
        VNode *v_freef = nullptr;
        VNode **v_freeb = nullptr;
        VNode *v_mountf = nullptr;
        VNode **v_mountb = nullptr;
        long v_numoutput = 0;
        enum vtype v_type = VNON;
        enum vtagtype v_tag = VT_NON;
        dev_t v_rdev = 0;

        // for special device alias list
        VNode *v_specnext = nullptr;
        VNode **v_hashchain = nullptr;

        /* Private data for filesystem */
        void *v_data = nullptr;

        VNode() = default;
        ~VNode() = default;

        static int vn_open(Nameidata &ndp, Sys::Process &p, int fmode, int cmode);
        int vn_close(int flags, const Sys::UserCredentials &cred, Sys::Process &p);
        int rdwr(enum uio_rw rw, void *base, int len, off_t offset,
                 enum uio_seg segflg, int ioflg,
                 const Sys::UserCredentials &cred, int *aresid,
                 Sys::Process &p);
        int vn_read(File *fp, struct uio *uio, const Sys::UserCredentials &cred);
        int vn_write(File *fp, struct uio *uio, const Sys::UserCredentials &cred);
        int vn_stat(struct stat *sb, Sys::Process &p);
        int vn_ioctl(File *fp, unsigned int com, void *data, Sys::Process &p);
        int vn_select(File *fp, int which, Sys::Process &p);
        int vn_closefile(File *fp, Sys::Process &p);
        [[nodiscard]] int writechk() const;
        int fhtovp(fhandle_t *fhp, int lockflag);
    };

/*
 * vnode flags
 */
#define VROOT    0x0001
#define VTEXT    0x0002
#define VSYSTEM  0x0004
#define VXLOCK   0x0100
#define VXWANT   0x0200
#define VBWAIT   0x0400
#define VALIASED 0x0800

/*
 * Modes.
 */
#define VSUID  04000
#define VSGID  02000
#define VSVTX  01000
#define VREAD  0400
#define VWRITE 0200
#define VEXEC  0100

/*
 * Token indicating no attribute value yet assigned.
 */
#define VNOVAL ((unsigned) 0xffffffff)

/*
 * Flags to various vnode functions.
 */
#define SKIPSYSTEM 0x0001
#define FORCECLOSE 0x0002
#define DOCLOSE    0x0004

/*
 * Inline reference counting macros.
 */
#define VREF(vp)            \
    do {                    \
        (vp)->v_usecount++; \
    } while (0)
#define VHOLD(vp)          \
    do {                   \
        (vp)->v_holdcnt++; \
    } while (0)
#define HOLDRELE(vp)       \
    do {                   \
        (vp)->v_holdcnt--; \
    } while (0)
#define VATTR_NULL(vap) VFS::vattr_null(vap)

#define NULLVP (static_cast<VFS::VNode *>(nullptr))

#define IO_UNIT       0x0001
#define IO_APPEND     0x0002
#define IO_NDELAY     0x0004
#define IO_NODELOCKED 0x0008

    /* Global vnode data */
    extern VNode *rootdir;
    extern long desiredvnodes;
    extern VFS::VAttr va_null;

} // namespace VFS

#endif // VFS_VNODE_HXX
