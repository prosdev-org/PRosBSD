#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#include <libkxx/heap.hxx>
#include <libkxx/string.hxx>
#include <libkxx/vector.hxx>

#include <sys/process.hxx>
// needed, but in every vfsops process is... SKIPPED :D
// TODO: fix it (obviously)

#include <unique/assert.h>
#include <unique/log.hxx>
#include <vfs/name_cache.hxx>
#include <vfs/ramfs.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vnode.hxx>

namespace VFS::RamFS {

    static kxx::StringView get_logger_prefix() {
        return "vfs/ramfs";
    }

    struct RamFSNode;

    struct RamFSMount {
        RamFSNode *root = nullptr;
    };

    struct RamFSNode {
        uint64_t ino = 0;
        enum vtype type = VREG;
        mode_t mode = 0;

        uid_t uid = 0;
        gid_t gid = 0;
        nlink_t nlink = 1;

        unsigned long long size = 0;
        kxx::Vector<char> data;

        RamFSNode *parent = nullptr;
        kxx::Vector<RamFSNode *> children;
        kxx::Vector<kxx::String> child_names;

        kxx::String name;
        kxx::String symlink_target;
        VNode *vnode = nullptr;
        bool removed = false;
    };

    static uint64_t next_ino = 1;

    // clion, what the heck?
    struct RamFSDirent {
        ino_t d_ino;
        uint16_t d_reclen;
        uint8_t d_type;
        uint8_t d_namlen;
    };

    static size_t ramfs_dirent_size(const size_t namelen) {
        const size_t raw = sizeof(RamFSDirent) + namelen + 1;
        return (raw + 3) & ~static_cast<size_t>(3);
    }

    class RamFSVFSOps final : public VFSOps {
    public:
        int mount(Mount &mp, char *path, void *data,
                  Nameidata &ndp, Sys::Process &p) override;

        int start(Mount &mp, int flags, Sys::Process &p) override;
        int unmount(Mount &mp, int mntflags, Sys::Process &p) override;
        int root(Mount &mp, VNode *&vpp) override;
        int statfs(Mount &mp, struct statfs &sbp, Sys::Process &p) override;
        int sync(Mount &mp, int waitfor) override;
        int fhtovp(Mount &mp, struct fid *fhp, VNode *&vpp) override;
        int vptofh(VNode &vp, struct fid *fhp) override;
        int init() override;
    };

    class RamFSVNodeOps final : public VNodeOps {
    public:
        int lookup(VNode &vp, Nameidata &ndp, Sys::Process &p) override;
        int create(Nameidata &ndp, VAttr &vap, Sys::Process &p) override;
        int mknod(Nameidata &ndp, VAttr &vap, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int open(VNode &vp, int mode, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int close(VNode &vp, int fflag, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int access(VNode &vp, int mode, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int getattr(VNode &vp, VAttr &vap, const Sys::UserCredentials &cred) override;
        int setattr(VNode &vp, VAttr &vap, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int read(VNode &vp, struct uio *uio, int ioflag, const Sys::UserCredentials &cred) override;
        int write(VNode &vp, struct uio *uio, int ioflag, const Sys::UserCredentials &cred) override;
        int ioctl(VNode &vp, unsigned int command, void *data, int fflag, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int select(VNode &vp, int which, int fflag, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int mmap(VNode &vp, int fflags, const Sys::UserCredentials &cred, Sys::Process &p) override;
        int fsync(VNode &vp, int fflags, const Sys::UserCredentials &cred, int waitfor, Sys::Process &p) override;
        int seek(VNode &vp, off_t oldoff, off_t newoff, const Sys::UserCredentials &cred) override;
        int remove(Nameidata &ndp, Sys::Process &p) override;
        int link(VNode &vp, Nameidata &ndp, Sys::Process &p) override;
        int rename(Nameidata &fndp, Nameidata &tndp, Sys::Process &p) override;
        int mkdir(Nameidata &ndp, VAttr &vap, Sys::Process &p) override;
        int rmdir(Nameidata &ndp, Sys::Process &p) override;
        int symlink(Nameidata &ndp, VAttr &vap, const char *target, Sys::Process &p) override;
        int readdir(VNode &vp, struct uio *uio, const Sys::UserCredentials &cred, int *eofflagp) override;
        int readlink(VNode &vp, struct uio *uio, const Sys::UserCredentials &cred) override;
        int abortop(Nameidata &ndp) override;
        int inactive(VNode &vp, Sys::Process &p) override;
        int reclaim(VNode &vp) override;
        int lock(VNode &vp) override;
        int unlock(VNode &vp) override;
        int bmap(VNode &vp, daddr_t bn, VNode &vpp, daddr_t *bnp) override;
        int strategy(BUF &bp) override;
        int print(VNode &vp) override;
        int islocked(VNode &vp) override;
        int advlock(VNode &vp, void *id, int op, FLock &fl, int flags) override;
    };

    static RamFSVFSOps ramfs_vfsops_instance;
    static RamFSVNodeOps ramfs_vnodeops_instance;

    static bool name_equals(const kxx::String &str, const char *data, const size_t len) {
        if (str.get_size() != len) {
            return false;
        }

        for (size_t i = 0; i < len; i++) {
            if (str.get(i) != data[i]) {
                return false;
            }
        }

        return true;
    }

    static RamFSNode *find_child(RamFSNode *dir, const char *name, const size_t len) {
        ASSERT(dir != nullptr);
        ASSERT(dir->type == VDIR);

        for (size_t i = 0; i < dir->children.get_size(); i++) {
            RamFSNode *child = dir->children.get(i);

            if (name_equals(dir->child_names.get(i), name, len)) {
                return child;
            }
        }

        return nullptr;
    }

    static void add_child(RamFSNode *dir, RamFSNode *node, const char *name, size_t name_len) {
        ASSERT(dir != nullptr);
        ASSERT(node != nullptr);
        ASSERT(name != nullptr);

        if (node->type == VDIR) {
            node->parent = dir;
        }
        dir->children.push_back(node);
        dir->child_names.push_back(kxx::String(name, name_len));
    }

    static void detach_child(RamFSNode *dir, RamFSNode *node, const char *name, size_t name_len) {
        ASSERT(dir != nullptr);
        ASSERT(node != nullptr);
        ASSERT(name != nullptr);

        for (size_t i = 0; i < dir->children.get_size(); i++) {
            if (dir->children.get(i) == node &&
                name_equals(dir->child_names.get(i), name, name_len)) {
                for (size_t j = i; j + 1 < dir->children.get_size(); j++) {
                    dir->children.set(dir->children.get(j + 1), j);
                    dir->child_names.set(dir->child_names.get(j + 1), j);
                }

                dir->children.pop_back();
                dir->child_names.pop_back();
                if (node->type == VDIR && node->parent == dir) {
                    node->parent = nullptr;
                }
                return;
            }
        }
    }

    static void destroy_node(RamFSNode *node) {
        if (node == nullptr) {
            return;
        }

        for (size_t i = 0; i < node->children.get_size(); i++) {
            destroy_node(node->children.get(i));
        }

        delete node;
    }

    static void delete_or_mark_node(RamFSNode *node) {
        ASSERT(node != nullptr);

        if (node->nlink != 0) {
            return;
        }

        node->removed = true;
        if (node->vnode == nullptr) {
            delete node;
        }
    }

    static void resize_file(RamFSNode *node, const size_t new_size) {
        ASSERT(node != nullptr);

        size_t cur = node->data.get_size();

        while (cur < new_size) {
            node->data.push_back(0);
            cur++;
        }

        while (cur > new_size) {
            node->data.pop_back();
            cur--;
        }

        node->size = new_size;
    }

    static bool is_descendant(RamFSNode *old, RamFSNode *node) {
        for (RamFSNode *p = node; p != nullptr; p = p->parent) {
            if (p == old) {
                return true;
            }
        }

        return false;
    }

    static int ramfs_vget(Mount *mp, RamFSNode *node, VNode *&vpp) {
        ASSERT(mp != nullptr);
        ASSERT(node != nullptr);

        if (node->vnode != nullptr) {
            vget(node->vnode);
            vpp = node->vnode;
            return 0;
        }

        VNode *vp = nullptr;

        int error = getnewvnode(VT_RAMFS, mp, &ramfs_vnodeops_instance, vp);
        if (error != 0) {
            vpp = nullptr;
            return error;
        }

        vp->v_type = node->type;
        vp->v_data = node;
        node->vnode = vp;

        vpp = vp;
        return 0;
    }

    static int uiomove_ramfs(RamFSNode *node, struct uio *uio) {
        ASSERT(node != nullptr);
        ASSERT(uio != nullptr);

        if (uio->uio_offset < 0 || uio->uio_resid < 0 || uio->uio_iovcnt < 0 ||
            uio->uio_iovcnt > UIO_MAXIOV || uio->uio_iov == nullptr) {
            return EINVAL;
        }

        unsigned long long off = static_cast<unsigned long long>(uio->uio_offset);
        int resid = uio->uio_resid;

        if (uio->uio_rw == UIO_READ) {
            if (off >= node->size) {
                // nothing, just EOF
                return 0;
            }
        } else if (uio->uio_rw == UIO_WRITE) {
            (void) 0;
        } else {
            return EINVAL;
        }

        for (int i = 0; i < uio->uio_iovcnt && resid > 0; i++) {
            char *base = static_cast<char *>(uio->uio_iov[i].iov_base);
            int len = uio->uio_iov[i].iov_len;
            if (len < 0)
                return EINVAL;
            if (len > resid)
                len = resid;
            if (len > 0 && base == nullptr)
                return EFAULT;

            if (len <= 0) {
                continue;
            }

            if (uio->uio_rw == UIO_READ) {
                if (off >= node->size) {
                    break;
                }

                const unsigned long long avail = node->size - off;
                const int n = (len > static_cast<int>(avail)) ? static_cast<int>(avail) : len;

                for (int j = 0; j < n; j++) {
                    base[j] = node->data.get(static_cast<size_t>(off + static_cast<unsigned long long>(j)));
                }

                off += static_cast<unsigned long long>(n);
                resid -= n;
            } else {
                if (off > static_cast<unsigned long long>(SIZE_MAX - static_cast<size_t>(len))) {
                    return EFBIG;
                }
                const size_t start = static_cast<size_t>(off);
                const size_t end = start + static_cast<size_t>(len);

                if (end > node->size) {
                    resize_file(node, end);
                }

                for (int j = 0; j < len; j++) {
                    node->data.set(base[j], start + static_cast<size_t>(j));
                }

                off += static_cast<unsigned long long>(len);
                resid -= len;
            }
        }

        uio->uio_offset = static_cast<off_t>(off);
        uio->uio_resid = resid;

        return 0;
    }

    int RamFSVFSOps::mount(Mount &mp, char *path, void *data,
                           Nameidata &ndp, Sys::Process &p) {
        (void) data;
        (void) ndp;
        (void) p;

        LOG("ramfs mounted (yay)");

        auto *mnt = new RamFSMount();
        auto *root = new RamFSNode();

        root->ino = next_ino++;
        root->type = VDIR;
        root->mode = 0755;
        root->uid = 0;
        root->gid = 0;
        root->nlink = 2;
        root->name = kxx::String("/");

        mnt->root = root;

        mp.mnt_data = mnt;

        memset(&mp.mnt_stat, 0, sizeof(mp.mnt_stat));
        mp.mnt_stat.f_type = MOUNT_RAMFS;
        mp.mnt_stat.f_fsize = 1;
        mp.mnt_stat.f_bsize = 1024;

        strncpy(mp.mnt_stat.f_mntonname, path, MNAMELEN - 1);
        strncpy(mp.mnt_stat.f_mntfromname, "ramfs", MNAMELEN - 1);

        return 0;
    }

    int RamFSVFSOps::start(Mount &mp, int flags, Sys::Process &p) { // well
        (void) mp;
        (void) flags;
        (void) p;

        return 0;
    }

    int RamFSVFSOps::unmount(Mount &mp, int mntflags, Sys::Process &p) {
        (void) p;

        auto *mnt = static_cast<RamFSMount *>(mp.mnt_data);
        ASSERT(mnt != nullptr);

        const int flags = (mntflags & MNT_FORCE) ? FORCECLOSE : 0;

        int error = vflush(&mp, nullptr, flags);
        if (error != 0) {
            return error;
        }

        destroy_node(mnt->root);
        delete mnt;

        mp.mnt_data = nullptr;

        return 0;
    }

    int RamFSVFSOps::root(Mount &mp, VNode *&vpp) {
        auto *mnt = static_cast<RamFSMount *>(mp.mnt_data);
        ASSERT(mnt != nullptr);

        return ramfs_vget(&mp, mnt->root, vpp);
    }

    int RamFSVFSOps::statfs(Mount &mp, struct statfs &sbp, Sys::Process &p) {
        (void) p;

        sbp = mp.mnt_stat;
        sbp.f_flags = mp.mnt_flag & MNT_VISFLAGMASK;

        return 0;
    }

    int RamFSVFSOps::sync(Mount &mp, int waitfor) {
        (void) mp;
        (void) waitfor;

        return 0;
    }

    int RamFSVFSOps::fhtovp(Mount &mp, struct fid *fhp, VNode *&vpp) {
        (void) mp;
        (void) fhp;
        (void) vpp;

        return EINVAL;
    }

    int RamFSVFSOps::vptofh(VNode &vp, struct fid *fhp) {
        (void) vp;
        (void) fhp;

        return EINVAL;
    }

    int RamFSVFSOps::init() {
        return 0;
    }

    int RamFSVNodeOps::lookup(VNode &vp, Nameidata &ndp, Sys::Process &p) {
        (void) p;

        auto *dir = static_cast<RamFSNode *>(vp.v_data);
        if (dir == nullptr || dir->type != VDIR) {
            return ENOTDIR;
        }

        if (ndp.ni_namelen == 1 && ndp.ni_ptr[0] == '.') {
            VREF(&vp);
            ndp.ni_vp = &vp;
            return 0;
        }

        if (ndp.ni_namelen == 2 &&
            ndp.ni_ptr[0] == '.' &&
            ndp.ni_ptr[1] == '.') {
            RamFSNode *parent = (dir->parent != nullptr) ? dir->parent : dir;
            return ramfs_vget(vp.v_mount, parent, ndp.ni_vp);
        }

        RamFSNode *child = find_child(
                dir,
                ndp.ni_ptr,
                static_cast<size_t>(ndp.ni_namelen));

        if (child == nullptr) {
            return ENOENT;
        }

        return ramfs_vget(vp.v_mount, child, ndp.ni_vp);
    }

    static int check_dir_modify(RamFSNode *dir, VNode *dvp, const Sys::UserCredentials &cred, Sys::Process &p) {
        if (dir == nullptr || dvp == nullptr || dir->type != VDIR) {
            return ENOTDIR;
        }
        return dvp->ops->access(*dvp, VWRITE | VEXEC, cred, p);
    }

    int RamFSVNodeOps::create(Nameidata &ndp, VAttr &vap, Sys::Process &p) {
        VNode *dvp = ndp.ni_dvp;
        if (dvp == nullptr) {
            return EINVAL;
        }

        auto *dir = static_cast<RamFSNode *>(dvp->v_data);
        int error = check_dir_modify(dir, dvp, p.get_user_credentials(), p);
        if (error != 0) {
            return error;
        }

        if (find_child(dir, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen)) != nullptr) {
            return EEXIST;
        }

        auto *node = new RamFSNode();

        node->ino = next_ino++;
        node->type = VREG;
        node->mode = (vap.va_mode == static_cast<mode_t>(VNOVAL))
                             ? 0644
                             : (vap.va_mode & 07777);

        node->uid = p.get_user_credentials().user_id;
        node->gid = p.get_user_credentials().groups.get_size() != 0
                            ? p.get_user_credentials().groups.get(0)
                            : 0;
        node->nlink = 1;
        node->name = kxx::String(ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));

        add_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));

        error = ramfs_vget(dvp->v_mount, node, ndp.ni_vp);
        if (error != 0) {
            detach_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
            delete node;
            ndp.ni_vp = nullptr;
        } else {
            NameCache::purge(*dvp);
        }

        return error;
    }

    int RamFSVNodeOps::mknod(Nameidata &ndp, VAttr &vap, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) ndp;
        (void) vap;
        (void) cred;
        (void) p;

        return ENOSYS;
    }

    int RamFSVNodeOps::open(VNode &vp, int mode, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) cred;
        (void) p;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr || node->removed) {
            return ENOENT;
        }

        if ((mode & FWRITE) && node->type == VDIR) {
            return EISDIR;
        }

        return 0;
    }

    int RamFSVNodeOps::close(VNode &vp, int fflag, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) vp;
        (void) fflag;
        (void) cred;
        (void) p;

        return 0;
    }

    int RamFSVNodeOps::access(VNode &vp, int mode, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) p;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }

        if ((mode & VWRITE) &&
            vp.v_mount != nullptr &&
            (vp.v_mount->mnt_flag & MNT_RDONLY)) {
            return EROFS;
        }

        if (cred.user_id == 0) {
            if ((mode & VEXEC) && node->type != VDIR && (node->mode & 0111) == 0) {
                return EACCES;
            }
            return 0;
        }

        gid_t gid = cred.groups.get_size() != 0 ? cred.groups.get(0) : static_cast<gid_t>(VNOVAL);
        mode_t bits;
        if (cred.user_id == node->uid) {
            bits = (node->mode >> 6) & 07;
        } else if (gid == node->gid) {
            bits = (node->mode >> 3) & 07;
        } else {
            bits = node->mode & 07;
        }

        if ((mode & VREAD) && (bits & 04) == 0) {
            return EACCES;
        }

        if ((mode & VWRITE) && (bits & 02) == 0) {
            return EACCES;
        }

        if ((mode & VEXEC) && (bits & 01) == 0) {
            return EACCES;
        }

        return 0;
    }

    int RamFSVNodeOps::getattr(VNode &vp, VAttr &vap, const Sys::UserCredentials &cred) {
        (void) cred;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }

        vap = va_null;

        vap.va_type = node->type;
        vap.va_mode = node->mode;
        vap.va_nlink = node->nlink;
        vap.va_uid = node->uid;
        vap.va_gid = node->gid;
        vap.va_fsid = 1;
        vap.va_fileid = static_cast<long>(node->ino);
        vap.va_qsize = node->size;
        vap.va_blocksize = 1024;
        vap.va_atime.tv_sec = 0;
        vap.va_atime.tv_usec = 0;
        vap.va_mtime.tv_sec = 0;
        vap.va_mtime.tv_usec = 0;
        vap.va_ctime.tv_sec = 0;
        vap.va_ctime.tv_usec = 0;
        vap.va_gen = 0;
        vap.va_flags = 0;
        vap.va_rdev = 0;
        vap.va_qbytes = node->size;

        return 0;
    }

    int RamFSVNodeOps::setattr(VNode &vp, VAttr &vap, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) p;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }

        const bool owner = cred.user_id == node->uid;
        const bool root = cred.user_id == 0;

        if (!root) { // everything for root.
            if (vap.va_uid != static_cast<uid_t>(VNOVAL) || vap.va_gid != static_cast<gid_t>(VNOVAL)) {
                return EPERM;
            }

            if (vap.va_mode != static_cast<mode_t>(VNOVAL) && !owner) {
                return EPERM;
            }
        }

        if (vap.va_qsize != static_cast<unsigned long long>(VNOVAL) && node->type != VREG) {
            return EINVAL;
        }

        if (vap.va_mode != static_cast<mode_t>(VNOVAL)) {
            node->mode = vap.va_mode & 07777;
        }
        if (vap.va_uid != static_cast<uid_t>(VNOVAL)) {
            node->uid = vap.va_uid;
        }
        if (vap.va_gid != static_cast<gid_t>(VNOVAL)) {
            node->gid = vap.va_gid;
        }
        if (vap.va_qsize != static_cast<unsigned long long>(VNOVAL)) {
            resize_file(node, static_cast<size_t>(vap.va_qsize));
        }

        return 0;
    }

    int RamFSVNodeOps::read(VNode &vp, struct uio *uio, int ioflag, const Sys::UserCredentials &cred) {
        (void) ioflag;
        (void) cred;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }

        if (node->type != VREG) {
            return (node->type == VDIR) ? EISDIR : EINVAL;
        }

        return uiomove_ramfs(node, uio);
    }

    int RamFSVNodeOps::write(VNode &vp, struct uio *uio, int ioflag, const Sys::UserCredentials &cred) {
        (void) cred;

        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }

        if (node->type != VREG) {
            return (node->type == VDIR) ? EISDIR : EINVAL;
        }

        if (ioflag & IO_APPEND) {
            uio->uio_offset = static_cast<off_t>(node->size);
        }

        return uiomove_ramfs(node, uio);
    }

    int RamFSVNodeOps::ioctl(VNode &vp, unsigned int command, void *data, int fflag,
                             const Sys::UserCredentials &cred, Sys::Process &p) { // no devices at this time
        // (is it even possible?)
        (void) vp;
        (void) command;
        (void) data;
        (void) fflag;
        (void) cred;
        (void) p;

        return ENOTTY;
    }

    // TODO: implement.. all of it

    int RamFSVNodeOps::select(VNode &vp, int which, int fflag,
                              const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) vp;
        (void) which;
        (void) fflag;
        (void) cred;
        (void) p;

        return 0;
    }

    int RamFSVNodeOps::mmap(VNode &vp, int fflags, const Sys::UserCredentials &cred, Sys::Process &p) {
        (void) vp;
        (void) fflags;
        (void) cred;
        (void) p;

        return ENOSYS;
    }

    int RamFSVNodeOps::fsync(VNode &vp, int fflags, const Sys::UserCredentials &cred,
                             int waitfor, Sys::Process &p) {
        (void) vp;
        (void) fflags;
        (void) cred;
        (void) waitfor;
        (void) p;

        return 0;
    }

    int RamFSVNodeOps::seek(VNode &vp, off_t oldoff, off_t newoff, const Sys::UserCredentials &cred) {
        (void) oldoff;
        (void) cred;
        if (vp.v_type != VREG)
            return ESPIPE;
        if (newoff < 0)
            return EINVAL;
        return 0;
    }

    int RamFSVNodeOps::remove(Nameidata &ndp, Sys::Process &p) {
        VNode *dvp = ndp.ni_dvp;
        VNode *vp = ndp.ni_vp;

        if (dvp == nullptr || vp == nullptr) {
            return EINVAL;
        }

        auto *dir = static_cast<RamFSNode *>(dvp->v_data);
        auto *node = static_cast<RamFSNode *>(vp->v_data);
        if (dir == nullptr || node == nullptr) {
            return ENOENT;
        }
        int error = check_dir_modify(dir, dvp, p.get_user_credentials(), p);
        if (error != 0) {
            return error;
        }
        if (node->type == VDIR) {
            return EPERM;
        }

        detach_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        if (node->nlink > 0) {
            --node->nlink;
        }
        delete_or_mark_node(node);
        NameCache::purge(*dvp);
        if (vp->v_mount != nullptr) {
            NameCache::purge(*vp);
        }
        return 0;
    }

    int RamFSVNodeOps::link(VNode &vp, Nameidata &ndp, Sys::Process &p) {
        auto *node = static_cast<RamFSNode *>(vp.v_data);
        auto *dir = ndp.ni_dvp != nullptr ? static_cast<RamFSNode *>(ndp.ni_dvp->v_data) : nullptr;
        if (node == nullptr || dir == nullptr || dir->type != VDIR) {
            return EINVAL;
        }
        int error = check_dir_modify(dir, ndp.ni_dvp, p.get_user_credentials(), p);
        if (error != 0) {
            return error;
        }
        if (node->type == VDIR) {
            return EPERM;
        }
        if (node->removed) {
            return ENOENT;
        }
        if (find_child(dir, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen)) != nullptr) {
            return EEXIST;
        }
        if (node->nlink == static_cast<nlink_t>(-1)) {
            return EMLINK;
        }

        add_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        ++node->nlink;
        NameCache::purge(*ndp.ni_dvp);
        return 0;
    }

    int RamFSVNodeOps::rename(Nameidata &fndp, Nameidata &tndp, Sys::Process &p) {
        VNode *fvp = fndp.ni_vp;
        VNode *fdvp = fndp.ni_dvp;
        VNode *tdvp = tndp.ni_dvp;
        VNode *tvp = tndp.ni_vp;

        if (fvp == nullptr || fdvp == nullptr || tdvp == nullptr) {
            return EINVAL;
        }
        auto *src = static_cast<RamFSNode *>(fvp->v_data);
        auto *src_dir = static_cast<RamFSNode *>(fdvp->v_data);
        auto *dst_dir = static_cast<RamFSNode *>(tdvp->v_data);
        if (src == nullptr || src_dir == nullptr || dst_dir == nullptr ||
            src_dir->type != VDIR || dst_dir->type != VDIR) {
            return ENOTDIR;
        }

        int error = check_dir_modify(src_dir, fdvp, p.get_user_credentials(), p);
        if (error != 0)
            return error;
        if (src_dir != dst_dir) {
            error = check_dir_modify(dst_dir, tdvp, p.get_user_credentials(), p);
            if (error != 0)
                return error;
        }

        if (src->type == VDIR && is_descendant(src, dst_dir)) {
            return EINVAL;
        }

        RamFSNode *target = tvp != nullptr ? static_cast<RamFSNode *>(tvp->v_data) : find_child(dst_dir, tndp.ni_ptr, static_cast<size_t>(tndp.ni_namelen));
        if (target == src) {
            return 0;
        }

        if (target != nullptr) {
            if (target->type != src->type) {
                return target->type == VDIR ? EISDIR : ENOTDIR;
            }
            if (target->type == VDIR && target->children.get_size() != 0) {
                return ENOTEMPTY;
            }
            detach_child(dst_dir, target, tndp.ni_ptr, static_cast<size_t>(tndp.ni_namelen));
            if (target->type == VDIR) {
                if (dst_dir->nlink > 0)
                    --dst_dir->nlink;
                target->nlink = 0;
            } else if (target->nlink > 0) {
                --target->nlink;
            }
            delete_or_mark_node(target);
        }

        if (src->type == VDIR && src_dir != dst_dir) {
            if (src_dir->nlink > 0)
                --src_dir->nlink;
            ++dst_dir->nlink;
        }
        detach_child(src_dir, src, fndp.ni_ptr, static_cast<size_t>(fndp.ni_namelen));
        src->name = kxx::String(tndp.ni_ptr, static_cast<size_t>(tndp.ni_namelen));
        add_child(dst_dir, src, tndp.ni_ptr, static_cast<size_t>(tndp.ni_namelen));

        NameCache::purge(*fdvp);
        NameCache::purge(*tdvp);
        NameCache::purge(*fvp);
        if (tvp != nullptr)
            NameCache::purge(*tvp);
        return 0;
    }

    int RamFSVNodeOps::mkdir(Nameidata &ndp, VAttr &vap, Sys::Process &p) {
        VNode *dvp = ndp.ni_dvp;
        if (dvp == nullptr) {
            return EINVAL;
        }

        auto *dir = static_cast<RamFSNode *>(dvp->v_data);
        int error = check_dir_modify(dir, dvp, p.get_user_credentials(), p);
        if (error != 0) {
            return error;
        }

        if (find_child(dir, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen)) != nullptr) {
            // for everything
            return EEXIST;
        }

        auto *node = new RamFSNode();

        node->ino = next_ino++;
        node->type = VDIR;
        node->mode = (vap.va_mode == static_cast<mode_t>(VNOVAL))
                             ? 0755
                             : (vap.va_mode & 07777);

        node->uid = p.get_user_credentials().user_id;
        node->gid = p.get_user_credentials().groups.get_size() != 0
                            ? p.get_user_credentials().groups.get(0)
                            : 0;
        node->nlink = 2;
        node->name = kxx::String(ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));

        add_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        dir->nlink++;
        NameCache::purge(*dvp);

        return 0;
    }

    int RamFSVNodeOps::rmdir(Nameidata &ndp, Sys::Process &p) {
        VNode *dvp = ndp.ni_dvp;
        VNode *vp = ndp.ni_vp;
        if (dvp == nullptr || vp == nullptr)
            return EINVAL;
        auto *dir = static_cast<RamFSNode *>(dvp->v_data);
        auto *node = static_cast<RamFSNode *>(vp->v_data);
        if (dir == nullptr || node == nullptr)
            return ENOENT;
        int error = check_dir_modify(dir, dvp, p.get_user_credentials(), p);
        if (error != 0)
            return error;
        if (node->type != VDIR)
            return ENOTDIR;
        if (node->parent == nullptr || node->parent != dir)
            return EINVAL;
        if (node->children.get_size() != 0)
            return ENOTEMPTY;

        detach_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        if (dir->nlink > 0)
            --dir->nlink;
        node->nlink = 0;
        delete_or_mark_node(node);
        NameCache::purge(*dvp);
        NameCache::purge(*vp);
        return 0;
    }

    int RamFSVNodeOps::symlink(Nameidata &ndp, VAttr &vap, const char *target, Sys::Process &p) {
        VNode *dvp = ndp.ni_dvp;
        if (dvp == nullptr || target == nullptr)
            return EINVAL;

        auto *dir = static_cast<RamFSNode *>(dvp->v_data);
        int error = check_dir_modify(dir, dvp, p.get_user_credentials(), p);
        if (error != 0)
            return error;
        size_t target_len = strnlen(target, MAXPATHLEN);
        if (target_len >= MAXPATHLEN)
            return ENAMETOOLONG;
        if (find_child(dir, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen)) != nullptr)
            return EEXIST;

        auto *node = new RamFSNode();
        node->ino = next_ino++;
        node->type = VLNK;
        node->mode = (vap.va_mode == static_cast<mode_t>(VNOVAL)) ? 0777 : (vap.va_mode & 07777);
        node->uid = p.get_user_credentials().user_id;
        node->gid = p.get_user_credentials().groups.get_size() != 0 ? p.get_user_credentials().groups.get(0) : 0;
        node->nlink = 1;
        node->name = kxx::String(ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        node->symlink_target = kxx::String(target, target_len);
        node->size = target_len;
        for (size_t i = 0; i < target_len; ++i)
            node->data.push_back(target[i]);
        add_child(dir, node, ndp.ni_ptr, static_cast<size_t>(ndp.ni_namelen));
        NameCache::purge(*dvp);
        return 0;
    }

    int RamFSVNodeOps::readdir(VNode &vp, struct uio *uio, const Sys::UserCredentials &cred, int *eofflagp) {
        (void) cred;
        if (uio == nullptr || uio->uio_offset < 0 || uio->uio_iov == nullptr || uio->uio_iovcnt < 0 ||
            uio->uio_iovcnt > UIO_MAXIOV || uio->uio_resid < 0)
            return EINVAL;
        auto *dir = static_cast<RamFSNode *>(vp.v_data);
        if (dir == nullptr)
            return ENOENT;
        if (dir->type != VDIR)
            return ENOTDIR;

        const size_t count = dir->children.get_size();
        size_t index = static_cast<size_t>(uio->uio_offset);
        if (index > count + 2)
            index = count + 2;
        const size_t start_index = index;
        size_t resid = static_cast<size_t>(uio->uio_resid);

        while (index < count + 2) {
            RamFSNode *node;
            const char *name = nullptr;
            size_t namelen;
            uint8_t type;
            if (index == 0) {
                node = dir;
                name = ".";
                namelen = 1;
                type = VDIR;
            } else if (index == 1) {
                node = dir->parent != nullptr ? dir->parent : dir;
                name = "..";
                namelen = 2;
                type = VDIR;
            } else {
                node = dir->children.get(index - 2);
                name = nullptr;
                namelen = dir->child_names.get(index - 2).get_size();
                type = static_cast<uint8_t>(node->type);
            }

            const size_t reclen = ramfs_dirent_size(namelen);
            if (reclen > resid || reclen > UINT16_MAX || namelen > UINT8_MAX)
                break;
            char record[512];
            if (reclen > sizeof(record))
                return ENAMETOOLONG; // wanted too much

            memset(record, 0, reclen);
            auto *de = reinterpret_cast<RamFSDirent *>(record);
            de->d_ino = node->ino;
            de->d_reclen = static_cast<uint16_t>(reclen);
            de->d_type = type;
            de->d_namlen = static_cast<uint8_t>(namelen);
            for (size_t i = 0; i < namelen; ++i) {
                record[sizeof(RamFSDirent) + i] = name != nullptr ? name[i] : dir->child_names.get(index - 2).get(i);
            }

            size_t left = reclen;
            size_t src_off = 0;
            for (int i = 0; i < uio->uio_iovcnt && left != 0; ++i) {
                auto *dst = static_cast<char *>(uio->uio_iov[i].iov_base);
                const int room_i = uio->uio_iov[i].iov_len;
                if (room_i < 0)
                    return EINVAL;
                if (room_i == 0)
                    continue;
                const size_t n = static_cast<size_t>(room_i) < left ? static_cast<size_t>(room_i) : left;
                if (dst == nullptr)
                    return EFAULT;
                memmove(dst, record + src_off, n);
                left -= n;
                src_off += n;
                uio->uio_iov[i].iov_base = dst + n;
                uio->uio_iov[i].iov_len -= static_cast<int>(n);
            }
            if (left != 0) {
                if (index == start_index)
                    return EINVAL;
                break;
            }
            uio->uio_resid -= static_cast<int>(reclen);
            resid -= reclen;
            ++index;
        }
        uio->uio_offset = static_cast<off_t>(index);
        if (eofflagp != nullptr)
            *eofflagp = (index >= count + 2);
        return 0;
    }

    int RamFSVNodeOps::readlink(VNode &vp, struct uio *uio, const Sys::UserCredentials &cred) {
        (void) cred;
        auto *node = static_cast<RamFSNode *>(vp.v_data);
        if (node == nullptr) {
            return ENOENT;
        }
        if (node->type != VLNK) {
            return EINVAL;
        }
        return uiomove_ramfs(node, uio);
    }

    int RamFSVNodeOps::abortop(Nameidata &ndp) {
        (void) ndp;

        return 0;
    }

    int RamFSVNodeOps::inactive(VNode &vp, Sys::Process &p) {
        (void) vp;
        (void) p;

        // keep vnode assoc
        return 0;
    }

    int RamFSVNodeOps::reclaim(VNode &vp) {
        auto *node = static_cast<RamFSNode *>(vp.v_data);

        if (node != nullptr) {
            if (node->vnode == &vp) {
                node->vnode = nullptr;
            }

            if (node->removed) {
                delete node;
            }
        }

        vp.v_data = nullptr;

        return 0;
    }


    // TODO: fix on SMP
    //  example of
    int RamFSVNodeOps::lock(VNode &vp) {
        (void) vp;

        return 0;
    }

    int RamFSVNodeOps::unlock(VNode &vp) {
        (void) vp;

        return 0;
    }
    // how not to do

    // TODO: implement
    int RamFSVNodeOps::bmap(VNode &vp, daddr_t bn, VNode &vpp, daddr_t *bnp) {
        (void) vp;
        (void) bn;
        (void) vpp;
        (void) bnp;

        return ENOSYS;
    }

    int RamFSVNodeOps::strategy(BUF &bp) {
        (void) bp;

        return ENOSYS;
    }

    int RamFSVNodeOps::print(VNode &vp) {
        (void) vp;

        return 0;
    }

    int RamFSVNodeOps::islocked(VNode &vp) {
        (void) vp;

        return 0;
    }

    int RamFSVNodeOps::advlock(VNode &vp, void *id, int op, FLock &fl, int flags) {
        (void) vp;
        (void) id;
        (void) op;
        (void) fl;
        (void) flags;

        return ENOSYS;
    }

    void init() {
        LOG("registering fs");

        vfs_register(MOUNT_RAMFS, &ramfs_vfsops_instance);
    }


} // namespace VFS::RamFS
