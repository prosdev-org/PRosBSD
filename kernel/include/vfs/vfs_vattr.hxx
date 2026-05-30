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
 *	@(#)vfs_vattr.hxx
 */

#ifndef VFS_VATTR_HXX
#define VFS_VATTR_HXX

#include <vfs/vnode.h>
#include <sys/types.h>
#include <stdint.h>

namespace  VFS {
    class VAttr {
    public: // yeah, in public. But no choice. So frickin pluralism....
        enum vtype va_type; /* vnode type (for create) */
        unsigned short va_mode; /* files access mode and type */
        short va_nlink; /* number of references to file */
        uid_t va_uid; /* owner user id */
        gid_t va_gid; /* owner group id */
        long va_fsid; /* file system id (dev for now) */
        long va_fileid; /* file id */
        unsigned long long int va_qsize; /* file size in bytes */
        long va_blocksize; /* blocksize preferred for i/o */
        struct timeval va_atime; /* time of last access */
        struct timeval va_mtime; /* time of last modification */
        struct timeval va_ctime; /* time file changed */
        unsigned long va_gen; /* generation number of file */
        unsigned long va_flags; /* flags defined for file */
        dev_t va_rdev; /* device the special file represents */
        unsigned long long int va_qbytes; /* bytes of disk space held by file */
    };
}

#endif //VFS_VATTR_HXX