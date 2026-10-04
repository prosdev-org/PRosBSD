add_library(
    vfs OBJECT
    dead_vnode.cxx
    file.cxx
    mount.cxx
    vfs_cache.cxx
    vfs_lookup.cxx
    vfs_subr.cxx
    vfs_syscalls.cxx
    vfs_vnops.cxx

    ramfs.cxx
)
