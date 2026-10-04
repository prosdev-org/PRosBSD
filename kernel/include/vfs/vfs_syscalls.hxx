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

#ifndef VFS_SYSCALLS_HXX
#define VFS_SYSCALLS_HXX

#include <sys/types.h>
#include <vfs/mount.hxx>

namespace Sys {
    class Process;
}

namespace VFS {

    class VNode;
    int sys_mount(Sys::Process &p, void *uap, int *retval);
    int sys_unmount(Sys::Process &p, void *uap, int *retval);
    int dounmount(Mount *mp, int flags, Sys::Process &p);
    int sys_sync(Sys::Process &p, void *uap, int *retval);
    int sys_statfs(Sys::Process &p, void *uap, int *retval);
    int sys_fstatfs(Sys::Process &p, void *uap, int *retval);
    int sys_chdir(Sys::Process &p, void *uap, int *retval);
    int sys_fchdir(Sys::Process &p, void *uap, int *retval);
    int sys_chroot(Sys::Process &p, void *uap, int *retval);
    int sys_open(Sys::Process &p, void *uap, int *retval);
    int sys_close(Sys::Process &p, void *uap, int *retval);
    int sys_read(Sys::Process &p, void *uap, int *retval);
    int sys_write(Sys::Process &p, void *uap, int *retval);
    int sys_fstat(Sys::Process &p, void *uap, int *retval);
    int sys_creat(Sys::Process &p, void *uap, int *retval);
    int sys_mknod(Sys::Process &p, void *uap, int *retval);
    int sys_link(Sys::Process &p, void *uap, int *retval);
    int sys_symlink(Sys::Process &p, void *uap, int *retval);
    int sys_unlink(Sys::Process &p, void *uap, int *retval);
    int sys_lseek(Sys::Process &p, void *uap, int *retval);
    int sys_access(Sys::Process &p, void *uap, int *retval);
    int sys_stat(Sys::Process &p, void *uap, int *retval);
    int sys_lstat(Sys::Process &p, void *uap, int *retval);
    int sys_readlink(Sys::Process &p, void *uap, int *retval);
    int sys_chmod(Sys::Process &p, void *uap, int *retval);
    int sys_fchmod(Sys::Process &p, void *uap, int *retval);
    int sys_chown(Sys::Process &p, void *uap, int *retval);
    int sys_fchown(Sys::Process &p, void *uap, int *retval);
    int sys_utimes(Sys::Process &p, void *uap, int *retval);
    int sys_truncate(Sys::Process &p, void *uap, int *retval);
    int sys_ftruncate(Sys::Process &p, void *uap, int *retval);
    int sys_fsync(Sys::Process &p, void *uap, int *retval);
    int sys_rename(Sys::Process &p, void *uap, int *retval);
    int sys_mkdir(Sys::Process &p, void *uap, int *retval);
    int sys_rmdir(Sys::Process &p, void *uap, int *retval);
    int sys_getdirentries(Sys::Process &p, void *uap, int *retval);
    int sys_umask(Sys::Process &p, void *uap, int *retval);
    int sys_revoke(Sys::Process &p, void *uap, int *retval);

    int getvnode(FileContext &fdp, int fdes, File *&fpp);
    int chdirec(Nameidata &ndp, Sys::Process &p);

} // namespace VFS

#endif // VFS_SYSCALLS_HXX
