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

#ifndef VFS_NAME_CACHE_HXX
#define VFS_NAME_CACHE_HXX

#include <vfs/mount.hxx>

#define NCHNAMLEN 31

namespace VFS {

    class VNode;
    class Nameidata;

    struct NameCacheEntry {
        NameCacheEntry *hash_next = nullptr;
        NameCacheEntry **hash_prev = nullptr;
        NameCacheEntry *lru_next = nullptr;
        NameCacheEntry **lru_prev = nullptr;

        VNode *dvp = nullptr;
        unsigned long dvpid = 0;

        VNode *vp = nullptr;
        unsigned long vpid = 0;

        char nlen = 0;
        char name[NCHNAMLEN] = {};
    };

    struct NchStats {
        long ncs_goodhits = 0;
        long ncs_neghits = 0;
        long ncs_badhits = 0;
        long ncs_falsehits = 0;
        long ncs_miss = 0;
        long ncs_long = 0;
        long ncs_pass2 = 0;
        long ncs_2passes = 0;
    };

    namespace NameCache {

        void init();
        int lookup(Nameidata &ndp);
        void enter(Nameidata &ndp);
        void purge(VNode &vp);
        void purge_vfs(Mount &mp);
        void invalidate_vp(VNode &vp);

    } // namespace NameCache

} // namespace VFS

#endif // VFS_NAME_CACHE_HXX
