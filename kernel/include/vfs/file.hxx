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

#ifndef VFS_FILE_HXX
#define VFS_FILE_HXX

#include <sys/types.h>
#include <sys/uio.hxx>
#include <sys/user_credentials.hxx>

namespace Sys {
    class Process;
}

namespace VFS {

    class VNode;
    class File;

/*
 * Descriptor types.
 */
#define DTYPE_VNODE  1
#define DTYPE_SOCKET 2

    class FileOps {
    public:
        virtual ~FileOps() = default;

        virtual int read(File &fp, struct uio &uio, const Sys::UserCredentials &cred) = 0;
        virtual int write(File &fp, struct uio &uio, const Sys::UserCredentials &cred) = 0;
        virtual int ioctl(File &fp, unsigned int com, void *data, Sys::Process &p) = 0;
        virtual int select(File &fp, int which, Sys::Process &p) = 0;
        virtual int close(File &fp, Sys::Process &p) = 0;
    };

    class File {
    public:
        File *f_filef = nullptr;
        File **f_fileb = nullptr;

        short f_flag = 0;
        short f_type = 0;
        short f_count = 0;
        short f_msgcount = 0;

        Sys::UserCredentials f_cred = {};
        FileOps *f_ops = nullptr;

        off_t f_offset = 0;
        void *f_data = nullptr; /* VNode or socket */

        File() = default;
        ~File();
    };

    class FileContext {
    public:
        VNode *fd_cdir = nullptr; /* current directory */
        VNode *fd_rdir = nullptr; /* root directory */
        File **fd_ofiles = nullptr; /* file structures for open files */
        int fd_nfiles = 0; /* number of open files */
        mode_t fd_cmask = 022; /* mask for file creation */

        FileContext() = default;
        ~FileContext();

        /* Allocate a new file descriptor slot */
        int alloc_fd(int &indx);

        /* Remove and return File for descriptor, or nullptr */
        File *remove_fd(int indx);

        /* Free a file descriptor slot and delete File object */
        void free_fd(int indx);

        /* Get the File for a descriptor, or nullptr */
        File *get_file(int fd) const;
    };

    extern FileOps &vn_fileops;

} // namespace VFS

#endif // VFS_FILE_HXX
