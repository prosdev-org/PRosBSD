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

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>
#include "pathnames.h"
#include "states.h"

session_t *sessions = NULL;
enum runcom_mode runcom_mode = AUTO;
char did_multiuser_chroot = 0;
char *custom_runcom_path = NULL;
int clang = 1;
state_t requested_transition = state_runcom;
struct timespec dtrtime = {.tv_sec = 0, .tv_nsec = 250000};
enum init_power power_type = NONE;

/*
 * The mother of all processes.
 */
int main(int argc, char **argv) {
    // basic checks
    if (getuid() != 0 || getpid() != 1) {
        errno = EPERM;
        perror("init");
        return 1;
    } else {
        setsid();
    }
    openlog("bsdinit", LOG_CONS | LOG_PID, LOG_DAEMON); // right into /dev/console, cuz syslogd is NOT RUNNING (guess why)
#ifdef BSD
    if (setlogin("root") < 0)
        vconsole_log(WARNING, "setlogin failed");
#endif
    for (int i = 1; i < argc; i++) { // have no idea, how kernel/initramfs will set this
        if (strcmp(argv[i], "-s") == 0)
            requested_transition = state_single_user;
        else if (strcmp(argv[i], "-f") == 0)
            runcom_mode = FAST;
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            custom_runcom_path = argv[++i];
        }
    }

    /*
     * We catch or block signals rather than ignore them,
     * so that they get reset on exec.
     */
    sigset_t mask;
    struct sigaction sa;
    handle(badsys, SIGSYS, 0);
    handle(disaster, SIGABRT, SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGXCPU, SIGXFSZ, 0);
    handle(transition_handler, SIGHUP, SIGTERM, SIGTSTP, SIGINT, SIGUSR2, 0);
    handle(alrm_handler, SIGALRM, 0);
    sigfillset(&mask);
#ifdef BSD
    delset(&mask, SIGABRT, SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ, SIGHUP, SIGTERM, SIGTSTP, SIGALRM, SIGINT, SIGUSR2, 0);
#else
    sigdelset(&mask, SIGABRT);
    sigdelset(&mask, SIGFPE);
    sigdelset(&mask, SIGILL);
    sigdelset(&mask, SIGSEGV);
    sigdelset(&mask, SIGBUS);
    sigdelset(&mask, SIGSYS);
    sigdelset(&mask, SIGXCPU);
    sigdelset(&mask, SIGXFSZ);
    sigdelset(&mask, SIGHUP);
    sigdelset(&mask, SIGTERM);
    sigdelset(&mask, SIGTSTP);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGINT);
    sigdelset(&mask, SIGUSR2);
#endif
    sigprocmask(SIG_SETMASK, &mask, NULL);
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTIN, &sa, NULL);
    sigaction(SIGTTOU, &sa, NULL);

    /*
     * Paranoia.
     */
    close(0);
    close(1);
    close(2);

    transition(requested_transition);

    // never reached
    closelog();
    return 1;
}

// logging using vsyslog. Replaces warning(), emergency(), stall() from most BSDs
void vconsole_log(enum init_log type, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    switch (type) // if not one of those, then it's stall!
    {
        case WARNING:
            vsyslog(LOG_WARNING, fmt, ap);
            break;
        case EMERGENCY:
            vsyslog(LOG_EMERG, fmt, ap);
            break;
        default:
            vsyslog(LOG_NOTICE, fmt, ap);
            sleep(STALL_TIMEOUT);
            break;
    }
    va_end(ap);
}

/*
 * Associate a function with a signal handler.
 */
void handle(void (*handler)(int), ...) {
    va_list ap;
    int sig;
    struct sigaction sa;
    sigset_t mask_all;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigfillset(&mask_all);
    va_start(ap, handler);
    while ((sig = va_arg(ap, int)) != 0) {
        sa.sa_mask = mask_all;
        if (sig == SIGCHLD)
            sa.sa_flags = SA_NOCLDSTOP;
        sigaction(sig, &sa, NULL);
    }
    va_end(ap);
}

void delset(sigset_t *maskp, ...) {
    va_list ap;
    int sig;
    va_start(ap, maskp);
    while ((sig = va_arg(ap, int)) != 0)
        sigdelset(maskp, sig);
    va_end(ap);
}

void badsys(int sig) {
    static int count = 0;
    (void) sig;
    vconsole_log(WARNING, "bad SYS");
    if (++count < 25)
        return;
    disaster(SIGSYS);
}

void disaster(int sig) {
    vconsole_log(WARNING, "fatal signal %d", sig);
    sleep(STALL_TIMEOUT);
    _exit(sig);
}

void transition_handler(int sig) {
    switch (sig) {
        case SIGHUP:
            requested_transition = state_clean_ttys;
            break;
        case SIGTERM:
            requested_transition = state_death;
            break;
        case SIGTSTP:
            requested_transition = state_catatonia;
            break;
        case SIGINT:
            requested_transition = state_reboot;
            break;
        case SIGUSR2:
            requested_transition = state_shutdown;
            break;
        default:
            break;
    }
}

void alrm_handler(int sig) {
    (void) sig;
    clang = 1;
}

// State machine entry
void transition(state_t s) {
    if (!s)
        return;
    for (;;) {
        s = (state_t) (*s)();
        if (!s)
            break;
    }
}

session_t *find_session(pid_t pid) {
    for (session_t *sp = sessions; sp; sp = sp->se_next)
        if (sp->se_process == pid)
            return sp;
    return NULL;
}

/*
 * Start a session and allocate a controlling terminal.
 * Only called by children of init after forking.
 */
void setctty(const char *name) {
    int fd;
#ifdef BSD
    (void) revoke(name);
#endif
    nanosleep(&dtrtime, NULL);
    if ((fd = open(name, O_RDWR)) < 0) {
        vconsole_log(STALL, "can't open %s", name);
        _exit(1);
    }
#ifndef BSD
    if (ioctl(fd, TIOCSCTTY, 0) < 0)
        vconsole_log(WARNING, "can't set tty %s", name);
#endif
    if (login_tty(fd) < 0) {
        vconsole_log(STALL, "can't get %s for controlling terminal", name);
        _exit(2);
    }
}

void open_console(void) {
    int fd;
#ifdef BSD
    (void) revoke(PATH_CONSOLE);
#endif
    if ((fd = open(PATH_CONSOLE, O_RDWR | O_NONBLOCK)) >= 0) {
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
#ifndef BSD
        if (ioctl(fd, TIOCSCTTY, 1) < 0)
            vconsole_log(WARNING, "can't set console");
#endif
        if (login_tty(fd) == 0)
            return;
        close(fd);
    }
    if ((fd = open(PATH_NULL, O_RDWR)) < 0) {
        vconsole_log(EMERGENCY, "cannot open null device");
        _exit(1);
    }
    if (fd != 0)
        dup2(fd, 0);
    fd = open(PATH_LOG, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0)
        dup2(0, 1);
    else {
        if (fd != 1)
            dup2(fd, 1);
        close(fd);
    }
    dup2(1, 2);
}

void execute_script(char *argv[]) {
    struct sigaction sa;
    char *sh_argv[3 + SCRIPT_ARGV_SIZE + 1];
    const char *shell = get_shell();
    int sh_len = 1, i;

    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    open_console();
    sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);

    if (access(argv[0], X_OK) == 0) {
        execv(argv[0], argv);
        vconsole_log(WARNING, "can't directly exec %s", argv[0]);
    } else if (errno != EACCES)
        vconsole_log(WARNING, "can't access %s", argv[0]);

    sh_argv[0] = (char *) shell;
    if (strcmp(shell, PATH_BSHELL) == 0) {
#ifdef BSD // did i kill bsd shell args? IDK, lol
        sh_argv[1] = "-o";
        sh_argv[2] = "verify";
        sh_len = 3;
#endif
    }
    for (i = 0; i < SCRIPT_ARGV_SIZE && argv[i]; i++)
        sh_argv[sh_len + i] = argv[i];
    sh_argv[sh_len + i] = NULL;
    execv(shell, sh_argv);
    vconsole_log(STALL, "can't exec %s for %s", shell, argv[0]);
    _exit(1);
}

void collect_child(pid_t pid) {
    session_t *sp;
    if (!sessions)
        return;
    if (!(sp = find_session(pid)))
        return;
    sp->se_process = 0;
    if (sp->se_flags & SE_SHUTDOWN) {
        session_t *snext, *sprev;
        if ((sprev = sp->se_prev))
            sprev->se_next = sp->se_next;
        else
            sessions = sp->se_next;
        if ((snext = sp->se_next))
            snext->se_prev = sp->se_prev;
        free_session(sp);
        return;
    }
    if ((pid = start_getty(sp)) < 0) {
        requested_transition = state_clean_ttys;
        return;
    }
    sp->se_process = pid;
    gettimeofday(&sp->se_started, NULL);
}

void prepare_child_session(const char **shell, const char **argv) {
    // TODO: add user password ask, when libs are done
    // struct ttyent *typ = getttynam("console");
    // struct passwd *pp = getpwnam("root");
    char altshell[128];
    setctty(access(PATH_CONSTTY, F_OK) == 0 ? PATH_CONSTTY : PATH_CONSOLE);
    endttyent();
    endpwent();
    fprintf(stderr, "Enter pathname of shell or RETURN for %s: ", *shell);
    if (fgets(altshell, sizeof(altshell), stdin) && altshell[0] != '\n') {
        altshell[strcspn(altshell, "\n")] = 0;
        *shell = strdup(altshell);
        argv[0] = *shell;
    } else
        argv[0] = "-sh";
}
