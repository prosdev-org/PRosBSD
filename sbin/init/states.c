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

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/reboot.h>
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
#include <time.h>
#include <utmp.h>
#include <pwd.h>
#include <termios.h>
#include "pathnames.h"
#include "states.h"

#include <sys/syslog.h>

// lol
#ifdef BSD
#include <ttyent.h>
#else
static struct ttyent linux_ttytab[] = { // busybox settings
//    { "console", "/sbin/getty 0 console linux",      "linux", TTY_ON|TTY_IFCONSOLE, NULL },
    { "ttyS0",    "/bin/busybox getty 0 ttyS0 linux",         "linux", TTY_ON, NULL },
    { "tty1",    "/bin/busybox getty 0 tty1 linux",         "linux", TTY_ON, NULL },
    { "tty2",    "/bin/busybox getty 0 tty2 linux",         "linux", TTY_ON, NULL },
    { "tty3",    "/bin/busybox getty 0 tty3 linux",         "linux", TTY_ON, NULL },
    { "tty4",    "/bin/busybox getty 0 tty4 linux",         "linux", TTY_ON, NULL },
	{ "tty5",    "/bin/busybox getty 0 tty5 linux",         "linux", TTY_ON, NULL },
	{ "tty6",    "/bin/busybox getty 0 tty6 linux",         "linux", TTY_ON, NULL },
	{ "tty7",    "/bin/busybox getty 0 tty7 linux",         "linux", TTY_ON, NULL },
	{ "tty8",    "/bin/busybox getty 0 tty8 linux",         "linux", TTY_ON, NULL },
	{ "tty9",    "/bin/busybox getty 0 tty9 linux",         "linux", TTY_ON, NULL },
	{ "tty10",    "/bin/busybox getty 0 tty10 linux",         "linux", TTY_ON, NULL },
	{ "tty11",    "/bin/busybox getty 0 tty11 linux",         "linux", TTY_ON, NULL },
	{ "tty12",    "/bin/busybox getty 0 tty12 linux",         "linux", TTY_ON, NULL },
	{ "tty13",    "/bin/busybox getty 0 tty13 linux",         "linux", TTY_ON, NULL },
	{ "tty14",    "/bin/busybox getty 0 tty14 linux",         "linux", TTY_ON, NULL },
	{ "tty15",    "/bin/busybox getty 0 tty15 linux",         "linux", TTY_ON, NULL },
	{ "tty16",    "/bin/busybox getty 0 tty16 linux",         "linux", TTY_ON, NULL },
	{ "tty17",    "/bin/busybox getty 0 tty17 linux",         "linux", TTY_ON, NULL },
	{ "tty18",    "/bin/busybox getty 0 tty18 linux",         "linux", TTY_ON, NULL },
	{ "tty19",    "/bin/busybox getty 0 tty19 linux",         "linux", TTY_ON, NULL },
    { NULL, NULL, NULL, 0, NULL }
};
static int ttyent_idx = 0;

struct ttyent *getttynam(const char *name) {
	for (int i = 0; linux_ttytab[i].ty_name; i++)
		if (strcmp(linux_ttytab[i].ty_name, name) == 0) return &linux_ttytab[i];
	return NULL;
}
struct ttyent *getttyent(void) {
	if (linux_ttytab[ttyent_idx].ty_name == NULL) { ttyent_idx = 0; return NULL; }
	return &linux_ttytab[ttyent_idx++];
}
int setttyent(void) { ttyent_idx = 0; return 0; }
int endttyent(void) { ttyent_idx = 0; return 0; }
#endif

int do_setttyent(void) { endttyent(); return setttyent(); }

// session manage
session_t *new_session(session_t *sprev, struct ttyent *typ) {
	if ((typ->ty_status & TTY_ON) == 0 || !typ->ty_name || !typ->ty_getty) return NULL;
	session_t *sp = calloc(1, sizeof(session_t));
	if (!sp) return NULL;
	sp->se_flags = SE_PRESENT;
	if (typ->ty_status & TTY_IFEXISTS) sp->se_flags |= SE_IFEXISTS;
	if (typ->ty_status & TTY_IFCONSOLE) sp->se_flags |= SE_IFCONSOLE;
	if (asprintf(&sp->se_device, "%s%s", _PATH_DEV, typ->ty_name) < 0) { free(sp); return NULL; }
	if (!setupargv(sp, typ)) { free_session(sp); return NULL; }
	if (!sprev) { sessions = sp; sp->se_prev = NULL; }
	else { sprev->se_next = sp; sp->se_prev = sprev; }
	return sp;
}

void free_session(session_t *sp) {
	if (!sp) return;
	free(sp->se_device);
	if (sp->se_getty) {
		free(sp->se_getty);
		if (sp->se_getty_argv) {
			for (char **p = sp->se_getty_argv; *p; p++) free(*p);
			free(sp->se_getty_argv);
		}
	}
	if (sp->se_window) {
		free(sp->se_window);
		if (sp->se_window_argv) {
			for (char **p = sp->se_window_argv; *p; p++) free(*p);
			free(sp->se_window_argv);
		}
	}
	free(sp->se_type);
	free(sp);
}

char **construct_argv(char *command) { // not safe enough?
    if (!command) return NULL;
    
    char *cmd = strdup(command);
    if (!cmd) return NULL;

    char **argv = malloc(32 * sizeof(char *));
    if (!argv) { free(cmd); return NULL; }

    int argc = 0;
    const char *sep = " \t";
    char *token = strtok(cmd, sep);
    while (token && argc < 30) {
        argv[argc++] = strdup(token);
        token = strtok(NULL, sep);
    }
    argv[argc] = NULL;
    free(cmd);
    return argv;
}

int setupargv(session_t *sp, struct ttyent *typ) {
	if (!sp || !typ) return 0;
	if (sp->se_getty) {
		free(sp->se_getty);
		if (sp->se_getty_argv) {
			for (char **p = sp->se_getty_argv; *p; p++) free(*p);
			free(sp->se_getty_argv);
		}
	}
	if (asprintf(&sp->se_getty, "%s %s", typ->ty_getty, typ->ty_name) < 0) return 0;
	sp->se_getty_argv = construct_argv(sp->se_getty);
	if (!sp->se_getty_argv) { free(sp->se_getty); sp->se_getty = NULL; return 0; }
	if (typ->ty_window) {
		sp->se_window = strdup(typ->ty_window);
		sp->se_window_argv = construct_argv(sp->se_window);
		if (!sp->se_window_argv) { free(sp->se_window); sp->se_window = NULL; return 0; }
	}
	return 1;
}

const char *get_shell(void) {
	return custom_runcom_path ? custom_runcom_path : PATH_BSHELL;
}

pid_t start_getty(session_t *sp) {
	if (!sp || !sp->se_getty_argv || !sp->se_getty_argv[0]) return -1;
	const time_t now = time(NULL);
	int do_sleep = 0;
	if (now >= sp->se_started.tv_sec && now - sp->se_started.tv_sec < GETTY_SPACING) {
		if (++sp->se_nspace > GETTY_NSPACE) { sp->se_nspace = 0; do_sleep = 1; }
	} else sp->se_nspace = 0;

	pid_t pid = fork();
	if (pid < 0) { vconsole_log(WARNING, "can't fork for getty on %s", sp->se_device); return -1; }
	if (pid > 0) return pid;

	if (do_sleep) { vconsole_log(WARNING, "getty repeating too quickly on %s, sleeping", sp->se_device); sleep(GETTY_SLEEP); }
	if (sp->se_window) {
		pid_t wp = fork();
		if (wp == 0) {
			setsid();
			execv(sp->se_window_argv[0], sp->se_window_argv);
			_exit(6);
		}
		if (wp > 0) sleep(WINDOW_WAIT);
	}
	sigset_t mask; sigemptyset(&mask); sigprocmask(SIG_SETMASK, &mask, NULL);
	execv(sp->se_getty_argv[0], sp->se_getty_argv);
	vconsole_log(STALL, "can't exec getty %s for %s", sp->se_getty_argv[0], sp->se_device);
	_exit(8);
}

// state machine states
state_func_t state_single_user(void) {
	pid_t pid, wpid; int status;
	struct sigaction sa_ign = {.sa_handler = SIG_IGN}, o_tstp, o_hup;
	const char *shell = PATH_BSHELL;
	char *argv[2] = {NULL, NULL};

#ifdef BSD
	if (getsecuritylevel() > 0) setsecuritylevel(0);
#endif
	sigaction(SIGTSTP, &sa_ign, &o_tstp); sigaction(SIGHUP, &sa_ign, &o_hup);

	if ((pid = fork()) < 0) {
		vconsole_log(EMERGENCY, "can't fork single-user shell, retrying");
		while (waitpid(-1, NULL, WNOHANG) > 0){}
		sigaction(SIGTSTP, &o_tstp, NULL); sigaction(SIGHUP, &o_hup, NULL);
		return (state_func_t)state_single_user;
	}
	if (pid == 0) {
		prepare_child_session(&shell, (const char**)argv);
		sigset_t mask; sigemptyset(&mask); sigprocmask(SIG_SETMASK, &mask, NULL);
		setenv("PATH", INIT_PATH, 1);
		argv[1] = NULL;
		execv(shell, argv);
		execv(PATH_BSHELL, (char*[]){"sh", NULL});
		_exit(3);
	}

	requested_transition = 0;
	do {
		wpid = waitpid(-1, &status, WUNTRACED);
		if (wpid < 0) { if (errno == EINTR) continue; break; }
		collect_child(wpid);
		if (wpid == pid && WIFSTOPPED(status)) kill(pid, SIGCONT);
	} while (wpid != pid && !requested_transition);

	sigaction(SIGTSTP, &o_tstp, NULL); sigaction(SIGHUP, &o_hup, NULL);
	if (requested_transition) return (state_func_t)requested_transition;
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
		sigset_t s; sigfillset(&s); for (;;) sigsuspend(&s);
	}
	runcom_mode = FAST;
	return (state_func_t)state_runcom;
}

state_func_t run_script(const char *script) {
	pid_t pid; int status;
	char *argv[SCRIPT_ARGV_SIZE + 2] = { (char *)script, NULL, NULL, NULL };

	if ((pid = fork()) < 0) {
		vconsole_log(EMERGENCY, "can't fork for %s", script); sleep(3);
		return (state_func_t)state_single_user;
	}
	if (pid == 0) {
		char autoboot[] = "autoboot";
		argv[1] = (runcom_mode == AUTO) ? autoboot : NULL;
		execute_script(argv); sleep(3); _exit(1);
	}

	requested_transition = 0;
	for (;;) {
		pid_t wpid = waitpid(-1, &status, WUNTRACED);
		if (wpid < 0) { if (errno == EINTR) continue; vconsole_log(WARNING, "wait for %s failed", script); return (state_func_t)state_single_user; }
		collect_child(wpid);
		if (requested_transition == state_death) return (state_func_t)requested_transition;
		if (wpid == pid && WIFSTOPPED(status)) { vconsole_log(WARNING, "%s stopped, restarting", script); kill(pid, SIGCONT); continue; }
		if (wpid == pid) break;
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGTERM && requested_transition == state_catatonia) {
		sigset_t s; sigfillset(&s); for (;;) sigsuspend(&s);
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) return (state_func_t)state_single_user; // Admin! Grab your ass and fix rc script
	return NULL;
}

state_func_t state_runcom(void) {
	const char *rc = custom_runcom_path ? custom_runcom_path : PATH_RUNCOM;
	vconsole_log(WARNING, "%s starting...", rc);
	state_func_t next = run_script(rc);
	if (next) return next;
	vconsole_log(WARNING, "%s finished", rc);
	runcom_mode = AUTO;
	return (state_func_t)state_read_ttys;
}

state_func_t state_read_ttys(void) {
	session_t *sp = NULL, *snext; struct ttyent *typ;
	for (session_t *cur = sessions; cur; cur = snext) { snext = cur->se_next; free_session(cur); }
	sessions = NULL;
	do_setttyent();
	while ((typ = getttyent())) { session_t *ns = new_session(sp, typ); if (ns) sp = ns; }
	endttyent();
	return (state_func_t)state_multi_user;
}

state_func_t state_multi_user(void) {
#ifdef BSD
	if (getsecuritylevel() == 0) setsecuritylevel(1);
#endif
	requested_transition = 0;
	for (session_t *sp = sessions; sp; sp = sp->se_next) {
		if (!sp->se_process && !(sp->se_flags & SE_SHUTDOWN)) {
			pid_t pid = start_getty(sp);
			if (pid > 0) { sp->se_process = pid; gettimeofday(&sp->se_started, NULL); }
		}
	}
	while (!requested_transition) {
		int status; pid_t pid = waitpid(-1, &status, 0);
		if (pid > 0) collect_child(pid);
	}
	return (state_func_t)requested_transition;
}

state_func_t state_clean_ttys(void) {
	session_t *sp, *sprev; struct ttyent *typ;
	int idx = 0, devlen = strlen(PATH_DEV);
	for (sp = sessions; sp; sp = sp->se_next) sp->se_flags &= ~SE_PRESENT;
	do_setttyent();
	while ((typ = getttyent())) {
		++idx;
		for (sprev = NULL, sp = sessions; sp; sp = sp->se_next)
			if (sp->se_device && strcmp(typ->ty_name, sp->se_device + devlen) == 0) break;
		if (sp) {
			sp->se_flags |= SE_PRESENT; sp->se_index = idx;
			if ((typ->ty_status & TTY_ON) == 0 || !typ->ty_getty) {
				sp->se_flags |= SE_SHUTDOWN;
				if (sp->se_process) kill(sp->se_process, SIGHUP);
				continue;
			}
			sp->se_flags &= ~SE_SHUTDOWN;
			if (!setupargv(sp, typ)) {
				vconsole_log(WARNING, "can't parse getty for %s", sp->se_device);
				sp->se_flags |= SE_SHUTDOWN;
				if (sp->se_process) kill(sp->se_process, SIGHUP);
			} else sp->se_nspace = 0;
		} else new_session(sprev, typ);
	}
	endttyent();
	for (sp = sessions; sp; sp = sp->se_next)
		if (!(sp->se_flags & SE_PRESENT)) {
			sp->se_flags |= SE_SHUTDOWN;
			if (sp->se_process) kill(sp->se_process, SIGHUP);
		}
	return (state_func_t)state_multi_user;
}

state_func_t state_catatonia(void) {
	for (session_t *sp = sessions; sp; sp = sp->se_next) sp->se_flags |= SE_SHUTDOWN;
	return (state_func_t)state_multi_user;
}

state_func_t state_death(void) {
	session_t *sp; int i, status; pid_t pid;
	static const int sigs[] = {SIGHUP, SIGTERM, SIGKILL};
	for (sp = sessions; sp; sp = sp->se_next) sp->se_flags |= SE_SHUTDOWN;
	for (i = 0; i < 3; i++) {
		if (kill(-1, sigs[i]) < 0 && errno == ESRCH) return (state_func_t)state_single_user;
		clang = 0; alarm(DEATH_WATCH);
		do { if ((pid = waitpid(-1, &status, 0)) > 0) collect_child(pid); }
		while (!clang && errno != ECHILD);
		if (errno == ECHILD) return (state_func_t)state_single_user;
	}
	vconsole_log(WARNING, "some processes would not die; ps axl advised");
	return (state_func_t)state_single_user;
}

state_func_t state_done(void) {
	state_func_t shutdown = run_script(PATH_SHUTDOWN);
	if (!shutdown) vconsole_log(WARNING, "cannot execute rc.shutdown");

	for (session_t *sp = sessions; sp; sp = sp->se_next)
	{
		sp->se_flags |= SE_SHUTDOWN;
		if (sp->se_process > 1)
		{
			kill(sp->se_process, SIGTERM);
		}
	}

	// kill all processes, that still exists
	sleep(KILL_TIMEOUT);
	kill(-1, SIGTERM);

	sleep(KILL_TIMEOUT);
	kill(-1, SIGKILL);

	switch (power_type)
	{
	case SHUTDOWN:
#ifdef linux
		reboot(RB_POWER_OFF);
#endif
#ifdef BSD
		reboot(RB_POWERDOWN);
#endif
		break;
	case REBOOT:
		reboot(RB_AUTOBOOT);
		break;
	default:
		vconsole_log(WARNING, "state done without power type"); // impossible! No, i'm iron....
		return NULL;
		break;
	}

	for (;;); // waiting to kill itself
	return (state_func_t)state_done;
}

state_func_t state_reboot(void) {
	power_type = REBOOT;
	return (state_func_t)state_done;
}

state_func_t state_shutdown(void) {
	power_type = SHUTDOWN;
	return (state_func_t)state_done;
}