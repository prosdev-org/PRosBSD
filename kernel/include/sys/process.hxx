/*-
 * Copyright (c) 1986, 1989, 1991 The Regents of the University of California.
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
 *	@(#)proc.h	7.28 (Berkeley) 5/30/91
 */

#ifndef SYS_PROCESS_HXX
#define SYS_PROCESS_HXX

#include <libkxx/shared_ptr.hxx>
#include <sys/types.h>
#include <sys/user_credentials.hxx>

namespace Sys {
    class Process {
    public:
        enum Status {
            Sleeping = 1,
            Waiting = 2,
            Running = 3,
            Idling = 4,
            Zombie = 5,
            Stoping = 6,
        };

        struct Flags {
            bool system : 1;
        };

        // stub
        struct FileContext {
        };

        static Process init_proc0();

        [[nodiscard]] pid_t get_pid() const;
        [[nodiscard]] Status get_status() const;

        [[nodiscard]] const UserCredentials &get_user_credentials() const;
        [[nodiscard]] FileContext &get_file_context();

    private:
        Process(pid_t pid, Flags flags, Status status,
                const kxx::SharedPtr<UserCredentials> &creds,
                const kxx::SharedPtr<FileContext> &file_context);

        Flags flags;
        Status status;

        kxx::SharedPtr<UserCredentials> creds;
        kxx::SharedPtr<FileContext> file_context;
    };
} // namespace Sys

#endif
