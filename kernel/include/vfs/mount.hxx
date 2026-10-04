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

#ifndef VFS_MOUNT_HXX
#define VFS_MOUNT_HXX

#include <sys/types.h>

#define MNAMELEN 90
typedef signed long long int fsid_t; // 32-BIT ONLY. TODO: MAKE IT cross-platform

struct statfs {
    short f_type;
    short f_flags;
    long f_fsize;
    long f_bsize;
    long f_blocks;
    long f_bfree;
    long f_bavail;
    long f_files;
    long f_ffree;
    fsid_t f_fsid;
    long f_spare[9];
    char f_mntonname[MNAMELEN];
    char f_mntfromname[MNAMELEN];
};

/*
 * File system types.
 */
#define MOUNT_NONE    0
#define MOUNT_UFS     1
#define MOUNT_NFS     2
#define MOUNT_MFS     3
#define MOUNT_PC      4
#define MOUNT_RAMFS   5
#define MOUNT_MAXTYPE 5

/*
 * Mount flags.
 */
#define MNT_RDONLY      0x00000001
#define MNT_SYNCHRONOUS 0x00000002
#define MNT_NOEXEC      0x00000004
#define MNT_NOSUID      0x00000008
#define MNT_NODEV       0x00000010

/* exported mount flags */
#define MNT_EXPORTED 0x00000100
#define MNT_EXRDONLY 0x00000200

/* Flags set by internal operations */
#define MNT_LOCAL 0x00001000
#define MNT_QUOTA 0x00002000

/* Mask of flags visible to statfs() */
#define MNT_VISFLAGMASK 0x0000ffff

/* Filesystem control flags */
#define MNT_UPDATE  0x00010000
#define MNT_MLOCK   0x00100000
#define MNT_MWAIT   0x00200000
#define MNT_MPBUSY  0x00400000
#define MNT_MPWANT  0x00800000
#define MNT_UNMOUNT 0x01000000

/* Flags for various system call interfaces */
#define MNT_FORCE   1
#define MNT_NOFORCE 2
#define MNT_WAIT    1
#define MNT_NOWAIT  2

/*
 * File identifier.
 */
#define MAXFIDSZ 16
struct fid {
    unsigned short fid_len;
    unsigned short fid_reserved;
    char fid_data[MAXFIDSZ];
};

/*
 * Generic file handle
 */
struct fhandle {
    fsid_t fh_fsid;
    struct fid fh_fid;
};
typedef struct fhandle fhandle_t;

namespace VFS {

    class VNode;
    class VFSOps;

    class Mount {
    public:
        Mount *mnt_next = nullptr;
        Mount *mnt_prev = nullptr;
        VFSOps *mnt_op = nullptr;
        VNode *mnt_vnodecovered = nullptr;
        VNode *mnt_mounth = nullptr;
        int mnt_flag = 0;
        uid_t mnt_exroot = 0;
        struct statfs mnt_stat = {};
        void *mnt_data = nullptr;

        Mount() = default;
        ~Mount() = default;

        int lock();
        void unlock();
        int busy();
        int unbusy();
    };

} // namespace VFS

#endif // VFS_MOUNT_HXX
