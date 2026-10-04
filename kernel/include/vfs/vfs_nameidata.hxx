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

#ifndef VFS_NAMEIDATA_HXX
#define VFS_NAMEIDATA_HXX

#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.hxx>
#include <sys/user_credentials.hxx>
#include <vfs/vfs_types.hxx>

namespace VFS {
    class VNode;
}

namespace Sys {
    class Process;
}

/*
 * namei operations
 */
#define LOOKUP 0
#define CREATE 1
#define DELETE 2
#define RENAME 3
#define OPMASK 3

/* namei operational modifiers */
#define LOCKLEAF   0x0004
#define LOCKPARENT 0x0008
#define WANTPARENT 0x0010
#define NOCACHE    0x0020
#define FOLLOW     0x0040
#define NOFOLLOW   0x0000
#define MODMASK    0x00fc

/* Namei parameter descriptors */
#define NOCROSSMOUNT 0x0100
#define REMOTE       0x0200
#define HASBUF       0x0400
#define SAVENAME     0x0800
#define SAVESTART    0x1000
#define PARAMASK     0xff00

#define MAXSYMLINKS 8
#define MAXPATHLEN  1024

namespace VFS {

    class Nameidata {
    public:
        void *ni_dirp = nullptr;
        enum uio_seg ni_segflg = UIO_SYSSPACE;
        unsigned long ni_nameiop = 0;
        Sys::UserCredentials ni_cred = {};
        VFS::VNode *ni_startdir = nullptr;
        VFS::VNode *ni_rootdir = nullptr;
        VFS::VNode *ni_vp = nullptr;
        VFS::VNode *ni_dvp = nullptr;
        char *ni_pnbuf = nullptr;
        bool ni_pnbuf_owned = false;
        long ni_pathlen = 0;
        char *ni_ptr = nullptr;
        long ni_namelen = 0;
        char *ni_next = nullptr;
        unsigned long ni_hash = 0;
        unsigned char ni_loopcnt = 0;
        unsigned char ni_makeentry = 0;
        unsigned char ni_isdotdot = 0;
        unsigned char ni_more = 0;

        Nameidata() = default;
        ~Nameidata();
    };
} // namespace VFS

#endif // VFS_NAMEIDATA_HXX
