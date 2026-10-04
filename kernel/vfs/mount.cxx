#include <machine/cpu.hxx>
#include <sys/panic.hxx>
#include <vfs/mount.hxx>

namespace VFS {

    int Mount::lock() { // dirty hack
        while (mnt_flag & MNT_MLOCK) {
            mnt_flag |= MNT_MWAIT;
            Machine::Cpu::relax();
        }

        mnt_flag |= MNT_MLOCK;
        return 0;
    }

    void Mount::unlock() {
        if ((mnt_flag & MNT_MLOCK) == 0) {
            Sys::panic("unlock: not locked");
        }

        mnt_flag &= ~MNT_MLOCK;
        if (mnt_flag & MNT_MWAIT) {
            mnt_flag &= ~MNT_MWAIT;
        }
    }

    int Mount::busy() {
        while (mnt_flag & MNT_MPBUSY) {
            mnt_flag |= MNT_MPWANT;
            Machine::Cpu::relax();
        }

        if (mnt_flag & MNT_UNMOUNT) {
            return 1;
        }

        mnt_flag |= MNT_MPBUSY;
        return 0;
    }

    int Mount::unbusy() {
        if ((mnt_flag & MNT_MPBUSY) == 0) {
            Sys::panic("Mount::unbusy: not busy");
        }

        mnt_flag &= ~MNT_MPBUSY;
        if (mnt_flag & MNT_MPWANT) {
            mnt_flag &= ~MNT_MPWANT;
        }
        return 0;
    }

} // namespace VFS
