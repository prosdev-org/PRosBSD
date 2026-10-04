// for subroutine

#include <errno.h>
#include <vfs/vfs_vnops.hxx>

namespace VFS {

    class DeadVNodeOps final : public VNodeOps {
    public:
        int lookup(VNode &, Nameidata &, Sys::Process &) override {
            return ENOENT;
        }
        int create(Nameidata &, VAttr &, Sys::Process &) override {
            return ENOENT;
        }
        int mknod(Nameidata &, VAttr &, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int open(VNode &, int, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int close(VNode &, int, const Sys::UserCredentials &, Sys::Process &) override {
            return 0;
        }
        int access(VNode &, int, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int getattr(VNode &, VAttr &, const Sys::UserCredentials &) override {
            return ENOENT;
        }
        int setattr(VNode &, VAttr &, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int read(VNode &, struct uio *, int, const Sys::UserCredentials &) override {
            return ENOENT;
        }
        int write(VNode &, struct uio *, int, const Sys::UserCredentials &) override {
            return ENOENT;
        }
        int ioctl(VNode &, unsigned int, void *, int, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int select(VNode &, int, int, const Sys::UserCredentials &, Sys::Process &) override {
            return 0;
        }
        int mmap(VNode &, int, const Sys::UserCredentials &, Sys::Process &) override {
            return ENOENT;
        }
        int fsync(VNode &, int, const Sys::UserCredentials &, int, Sys::Process &) override {
            return 0;
        }
        int seek(VNode &, off_t, off_t, const Sys::UserCredentials &) override {
            return 0;
        }
        int remove(Nameidata &, Sys::Process &) override {
            return ENOENT;
        }
        int link(VNode &, Nameidata &, Sys::Process &) override {
            return ENOENT;
        }
        int rename(Nameidata &, Nameidata &, Sys::Process &) override {
            return ENOENT;
        }
        int mkdir(Nameidata &, VAttr &, Sys::Process &) override {
            return ENOENT;
        }
        int rmdir(Nameidata &, Sys::Process &) override {
            return ENOENT;
        }
        int symlink(Nameidata &, VAttr &, const char *, Sys::Process &) override {
            return ENOENT;
        }
        int readdir(VNode &, struct uio *, const Sys::UserCredentials &, int *) override {
            return ENOENT;
        }
        int readlink(VNode &, struct uio *, const Sys::UserCredentials &) override {
            return ENOENT;
        }
        int abortop(Nameidata &) override {
            return 0;
        }
        int inactive(VNode &, Sys::Process &) override {
            return 0;
        }
        int reclaim(VNode &) override {
            return 0;
        }
        int lock(VNode &) override {
            return 0;
        }
        int unlock(VNode &) override {
            return 0;
        }
        int bmap(VNode &, daddr_t, VNode &, daddr_t *) override {
            return ENOENT;
        }
        int strategy(BUF &) override {
            return ENOENT;
        }
        int print(VNode &) override {
            return 0;
        }
        int islocked(VNode &) override {
            return 0;
        }
        int advlock(VNode &, void *, int, FLock &, int) override {
            return ENOENT;
        }
    };

    DeadVNodeOps dead_vnodeops_instance;
    VNodeOps &dead_vnodeops = dead_vnodeops_instance;

} // namespace VFS
