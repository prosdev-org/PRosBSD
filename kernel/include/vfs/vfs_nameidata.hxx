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
 *	@(#)vfs_nameidata.hxx
 */

#ifndef VFS_NAMEIDATA_HXX
#define VFS_NAMEIDATA_HXX

#include <vfs/vnode.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/uio.hxx>
#include <vfs/namei.h>

namespace VFS {
    class VNode;
}

class Nameidata {
public:
    /*
 * Arguments to namei.
 */
    void *ni_dirp; /* pathname pointer */
    enum uio_seg ni_segflg; /* location of pathname */
    unsigned long ni_nameiop; /* see below */
    /*
     * Arguments to lookup.
     */
    Sys::Process &ni_cred; /* credentials */
    VFS::VNode &ni_startdir; /* starting directory */
    VFS::VNode &ni_rootdir; /* logical root directory */
    /*
     * Results
     */
    VFS::VNode *ni_vp; /* vnode of result */
    VFS::VNode *ni_dvp; /* vnode of intermediate directory */
    /*
     * Shared between namei, lookup routines, and commit routines.
     */
    char *ni_pnbuf; /* pathname buffer */
    long ni_pathlen; /* remaining chars in path */
    char *ni_ptr; /* current location in pathname */
    long ni_namelen; /* length of current component */
    char *ni_next; /* next location in pathname */
    unsigned long ni_hash; /* hash value of current component */
    unsigned char ni_loopcnt; /* count of symlinks encountered */
    unsigned char ni_makeentry; /* 1 => add entry to name cache */
    unsigned char ni_isdotdot; /* 1 => current component name is .. */
    unsigned char ni_more; /* 1 => symlink needs interpretation */
};


#endif //VFS_NAMEIDATA_HXX