#ifndef TYPES_H
#define TYPES_H

typedef long int off_t;
typedef int pid_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long int time_t; // TODO: fix 2038 year issue, when on 64-bit systems
typedef long int suseconds_t;
typedef unsigned long int dev_t;
typedef unsigned int mode_t;
typedef unsigned long nlink_t;
typedef long int ssize_t;
typedef long int ino_t;
typedef long int daddr_t;

struct timeval { // this gonna be placed here, until project grows really big
    time_t tv_sec;
    suseconds_t tv_usec;
};

struct itimerval { // same
    struct timeval it_interval;
    struct timeval it_value;
};

#endif // TYPES_H
