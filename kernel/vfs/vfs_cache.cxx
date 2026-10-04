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
#include <libkxx/vector.hxx>
#include <string.h>
#include <unique/log.hxx>
#include <vfs/name_cache.hxx>
#include <vfs/vfs_nameidata.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vnode.hxx>

#define NCHASH_SIZE 64

namespace VFS::NameCache {

    static kxx::StringView get_logger_prefix() {
        return "vfs/cache";
    }

    static NameCacheEntry *nchashtable[NCHASH_SIZE];
    static NameCacheEntry *nchhead = nullptr;
    static NameCacheEntry **nchtail = &nchhead;

    static long numcache = 0;
    static long desiredcache = 128;

    static NchStats nchstats;
    static bool doingcache = true;
    static unsigned long nextvnodeid = 1;

    static void hash_remove(NameCacheEntry *ncp) {
        if (ncp->hash_prev == nullptr) {
            return;
        }

        *ncp->hash_prev = ncp->hash_next;
        if (ncp->hash_next != nullptr) {
            ncp->hash_next->hash_prev = ncp->hash_prev;
        }

        ncp->hash_next = nullptr;
        ncp->hash_prev = nullptr;
    }

    static void hash_insert(NameCacheEntry *ncp, const unsigned long hash) {
        ncp->hash_next = nchashtable[hash];
        ncp->hash_prev = &nchashtable[hash];

        if (nchashtable[hash] != nullptr) {
            nchashtable[hash]->hash_prev = &ncp->hash_next;
        }

        nchashtable[hash] = ncp;
    }

    static void lru_remove(NameCacheEntry *ncp) {
        if (ncp->lru_prev == nullptr) {
            return;
        }

        *ncp->lru_prev = ncp->lru_next;
        if (ncp->lru_next != nullptr) {
            ncp->lru_next->lru_prev = ncp->lru_prev;
        } else {
            nchtail = ncp->lru_prev;
        }

        ncp->lru_next = nullptr;
        ncp->lru_prev = nullptr;
    }

    static void lru_append(NameCacheEntry *ncp) {
        ncp->lru_next = nullptr;
        ncp->lru_prev = nchtail;
        *nchtail = ncp;
        nchtail = &ncp->lru_next;
    }

    static void move_to_lru_tail(NameCacheEntry *ncp) {
        lru_remove(ncp);
        lru_append(ncp);
    }

    static void delete_entry(NameCacheEntry *ncp) {
        hash_remove(ncp);
        lru_remove(ncp);
        delete ncp;
        numcache--;
    }

    static void delete_all_entries() {
        for (size_t i = 0; i < NCHASH_SIZE; i++) {
            NameCacheEntry *ncp = nchashtable[i];

            while (ncp != nullptr) {
                NameCacheEntry *next = ncp->hash_next;
                delete_entry(ncp);
                ncp = next;
            }
        }

        nchhead = nullptr;
        nchtail = &nchhead;
    }

    void init() {
        LOG("initializing name cache");

        for (size_t i = 0; i < NCHASH_SIZE; i++) {
            nchashtable[i] = nullptr;
        }

        nchhead = nullptr;
        nchtail = &nchhead;
        numcache = 0;
    }

    // look for name in cache
    int lookup(Nameidata &ndp) {
        if (!doingcache) {
            return 0;
        }

        if (ndp.ni_namelen > NCHNAMLEN) {
            nchstats.ncs_long++;
            ndp.ni_makeentry = 0;
            return 0;
        }

        VNode *dvp = ndp.ni_dvp;
        if (dvp == nullptr) {
            return 0;
        }

        const unsigned long hash = ndp.ni_hash & (NCHASH_SIZE - 1);

        for (NameCacheEntry *ncp = nchashtable[hash]; ncp != nullptr;) {
            NameCacheEntry *next = ncp->hash_next;

            const bool dvp_ok =
                    (ncp->dvp == dvp) &&
                    (ncp->dvpid == dvp->v_id);

            const bool name_ok =
                    dvp_ok &&
                    (ncp->nlen == static_cast<char>(ndp.ni_namelen)) &&
                    (memcmp(ncp->name, ndp.ni_ptr, ncp->nlen) == 0);

            if (!name_ok) {
                ncp = next;
                continue;
            }

            // is entry stale?
            if (ncp->vp != nullptr && ncp->vpid != ncp->vp->v_id) {
                nchstats.ncs_falsehits++;
                delete_entry(ncp);
                ncp = next;
                continue;
            }

            // negative cache entry
            if (ncp->vp == nullptr) {
                if ((ndp.ni_nameiop & OPMASK) == CREATE) {
                    nchstats.ncs_miss++;
                    delete_entry(ncp);
                    return 0;
                }

                nchstats.ncs_neghits++;
                move_to_lru_tail(ncp);
                return ENOENT;
            }

            // positive
            nchstats.ncs_goodhits++;
            vget(ncp->vp);

            ndp.ni_vp = ncp->vp;
            move_to_lru_tail(ncp);
            return -1;
        }

        nchstats.ncs_miss++;
        return 0;
    }

    // add entry
    void enter(Nameidata &ndp) {
        if (!doingcache) {
            return;
        }

        if (ndp.ni_dvp == nullptr) {
            return;
        }

        if (ndp.ni_namelen > NCHNAMLEN) {
            return;
        }

        const unsigned long hash = ndp.ni_hash & (NCHASH_SIZE - 1);

        // no need to have same cache
        for (NameCacheEntry *ncp = nchashtable[hash]; ncp != nullptr;) {
            NameCacheEntry *next = ncp->hash_next;

            if (ncp->dvp == ndp.ni_dvp &&
                ncp->dvpid == ndp.ni_dvp->v_id &&
                ncp->nlen == static_cast<char>(ndp.ni_namelen) &&
                memcmp(ncp->name, ndp.ni_ptr, ncp->nlen) == 0) {
                delete_entry(ncp);
            }

            ncp = next;
        }

        NameCacheEntry *ncp = nullptr;

        if (numcache < desiredcache) {
            ncp = new NameCacheEntry();
            numcache++;
        } else {
            ncp = nchhead;
            if (ncp == nullptr) {
                return;
            }

            hash_remove(ncp);
            lru_remove(ncp);
        }

        ncp->dvp = ndp.ni_dvp;
        ncp->dvpid = ndp.ni_dvp->v_id;

        if (ndp.ni_vp != nullptr) {
            ncp->vp = ndp.ni_vp;
            ncp->vpid = ndp.ni_vp->v_id;
        } else {
            ncp->vp = nullptr;
            ncp->vpid = 0;
        }

        ncp->nlen = static_cast<char>(ndp.ni_namelen);
        memcpy(ncp->name, ndp.ni_ptr, ndp.ni_namelen);

        hash_insert(ncp, hash);
        lru_append(ncp);
    }

    // flush
    void purge(VNode &vp) {
        vp.v_id = ++nextvnodeid;

        if (nextvnodeid == 0) {
            delete_all_entries();
            nextvnodeid = 1;
            vp.v_id = nextvnodeid;
        }

        // invalidate both
        for (size_t i = 0; i < NCHASH_SIZE; i++) {
            NameCacheEntry *ncp = nchashtable[i];
            while (ncp != nullptr) {
                NameCacheEntry *next = ncp->hash_next;
                if (ncp->dvp == &vp || ncp->vp == &vp) {
                    delete_entry(ncp);
                }
                ncp = next;
            }
        }
    }

    void invalidate_vp(VNode &vp) {
        purge(vp);
    }

    // full flush!!!!!!!!!
    void purge_vfs(Mount &mp) {
        for (size_t i = 0; i < NCHASH_SIZE; i++) {
            NameCacheEntry *ncp = nchashtable[i];

            while (ncp != nullptr) {
                NameCacheEntry *next = ncp->hash_next;

                const bool dvp_belongs =
                        (ncp->dvp != nullptr) &&
                        (ncp->dvp->v_mount == &mp);

                const bool vp_belongs =
                        (ncp->vp != nullptr) &&
                        (ncp->vp->v_mount == &mp);

                if (dvp_belongs || vp_belongs) {
                    delete_entry(ncp);
                }

                ncp = next;
            }
        }
    }

} // namespace VFS::NameCache
