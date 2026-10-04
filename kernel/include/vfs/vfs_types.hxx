#ifndef VFS_TYPES_HXX
#define VFS_TYPES_HXX

#include <sys/types.h>

namespace VFS {

    enum vtype {
        VNON, // no vnode type
        VREG,
        VDIR,
        VBLK,
        VCHR,
        VLNK,
        VSOCK,
        VFIFO,
        VBAD
    };

    enum vtagtype {
        VT_NON,
        VT_UFS,
        VT_NFS,
        VT_MFS,
        VT_RAMFS
    };

} // namespace VFS

#endif // VFS_TYPES_HXX
