/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Donn Seeley at Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 */

#ifndef STATES_H
#define STATES_H

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <utmp.h>
#include <pwd.h>
#include <termios.h>
#include <crypt.h>

#ifndef TIOCSCTTY
#define TIOCSCTTY _IO('t', 19)
#endif

struct ttyent {
	char *ty_name;
	char *ty_getty;
	char *ty_type;
	int   ty_status;
	char *ty_window;
};

#define TTY_ON        0x01
#define TTY_OFF       0x02
#define TTY_IFEXISTS  0x10
#define TTY_IFCONSOLE 0x08
#define TTY_SECURE    0x20

#define	SE_SHUTDOWN	0x1
#define	SE_PRESENT	0x2
#define	SE_IFEXISTS	0x4
#define	SE_IFCONSOLE	0x8
#define SCRIPT_ARGV_SIZE 3

#define	GETTY_SPACING		 5
#define	GETTY_SLEEP		30
#define	GETTY_NSPACE		 5
#define	WINDOW_WAIT		 3
#define	STALL_TIMEOUT		30
#define KILL_TIMEOUT 1
#define	DEATH_WATCH		10

typedef struct init_session {
	int se_index;
	pid_t se_process;
	struct timeval se_started;
	int	se_nspace;
	int	se_flags;
	char *se_device;
	char *se_getty;
	char **se_getty_argv;
	char *se_window;
	char **se_window_argv;
	char *se_type;
	struct init_session *se_prev;
	struct init_session *se_next;
} session_t;

enum init_log
{
	WARNING,
	EMERGENCY,
	STALL
};

enum init_power
{
	NONE,
	SHUTDOWN,
	REBOOT
};

enum runcom_mode { AUTO, FAST };

typedef long (*state_func_t)(void);
typedef state_func_t (*state_t)(void);

extern session_t *sessions;
extern enum runcom_mode runcom_mode;
extern char did_multiuser_chroot;
extern char *custom_runcom_path;
extern state_t requested_transition;
extern struct timespec dtrtime;
extern enum init_power power_type;
extern int clang;

void handle(void (*handler)(int), ...);
void delset(sigset_t *maskp, ...);
void badsys(int sig);
void disaster(int sig);
void transition_handler(int sig);
void alrm_handler(int sig);
void transition(state_t s);
void setctty(const char *name);
void open_console(void);
void execute_script(char *argv[]);
void collect_child(pid_t pid);
void prepare_child_session(const char **shell, const char **argv);
void vconsole_log(enum init_log type, const char *fmt, ...);

state_func_t state_single_user(void);
state_func_t state_runcom(void);
state_func_t state_read_ttys(void);
state_func_t state_multi_user(void);
state_func_t state_clean_ttys(void);
state_func_t state_catatonia(void);
state_func_t state_death(void);
state_func_t state_done(void);
state_func_t state_reboot(void);
state_func_t state_shutdown(void);
state_func_t run_script(const char *script);

session_t *new_session(session_t *sprev, struct ttyent *typ);
session_t *find_session(pid_t pid);
void free_session(session_t *sp);
pid_t start_getty(session_t *sp);
char **construct_argv(char *command);
int setupargv(session_t *sp, struct ttyent *typ);
const char *get_shell(void);
int do_setttyent(void);

struct ttyent *getttynam(const char *name);
struct ttyent *getttyent(void);
int setttyent(void);
int endttyent(void);

#endif // STATES_H