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
 *	@(#)process.hxx
 */

#ifndef SYS_PROCESS_HXX
#define SYS_PROCESS_HXX

#include <libkxx/shared_ptr.hxx>
#include <sys/types.h>
#include <sys/user_credentials.hxx>

#define p_rlimit    p_limit->pl_rlimit
#define p_startzero p_ysptr
#define p_endzero   p_startcopy
#define p_startcopy p_sigmask
#define p_endcopy   p_wmesg
#define p_session   p_pgrp->pg_session
#define p_pgid      p_pgrp->pg_id

/*
 * Description of a process.
 * This class contains the information needed to manage a thread
 * of control, known in UN*X as a process; it has references to subclasses
 * containing descriptions of things that the process uses, but may share
 * with related processes.
 */
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


namespace VFS {
    class VNode;
} // namespace VFS
/*
 * One structure allocated per session.
 */
struct session {
    int s_count; /* ref cnt; pgrps in session */
    Sys::Process *s_leader; /* session leader */
    VFS::VNode *s_ttyvp; /* vnode of controlling terminal */
    // struct	tty *s_ttyp;		/* controlling terminal */
    char s_login[17]; /* setlogin() name */
};

/*
 * One structure allocated per process group.
 */
struct pgrp {
    struct pgrp *pg_hforw; /* forward link in hash bucket */
    struct proc *pg_mem; /* pointer to pgrp members */
    struct session *pg_session; /* pointer to session */
    pid_t pg_id; /* pgrp id */
    int pg_jobc; /* # procs qualifying pgrp for job control */
};

#endif
