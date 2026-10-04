#include <errno.h>
#include <fcntl.h>
#include <libkxx/heap.hxx>
#include <string.h>
#include <vfs/file.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_vnode.hxx>

namespace VFS {

    File::~File() {
        // free vnode if if not close()
        if (f_data != nullptr && f_type == DTYPE_VNODE) {
            auto *vp = static_cast<VNode *>(f_data);

            if (f_flag & FWRITE) {
                vp->v_writecount--;
            }

            vrele(vp);
            f_data = nullptr;
        }
    }

    FileContext::~FileContext() {
        if (fd_ofiles != nullptr) {
            for (int i = 0; i < fd_nfiles; i++) {
                if (fd_ofiles[i] != nullptr) {
                    delete fd_ofiles[i];
                    fd_ofiles[i] = nullptr;
                }
            }

            kxx::Heap::free(fd_ofiles);
            fd_ofiles = nullptr;
            fd_nfiles = 0;
        }

        if (fd_cdir != nullptr) {
            vrele(fd_cdir);
            fd_cdir = nullptr;
        }

        if (fd_rdir != nullptr) {
            vrele(fd_rdir);
            fd_rdir = nullptr;
        }
    }

    int FileContext::alloc_fd(int &indx) {
        // search for a free slot
        for (int i = 0; i < fd_nfiles; i++) {
            if (fd_ofiles[i] == nullptr) {
                indx = i;
                return 0;
            }
        }

        // grow the table
        const int new_nfiles = (fd_nfiles == 0) ? 16 : fd_nfiles * 2;

        auto **new_ofiles = static_cast<File **>(
                kxx::Heap::malloc(sizeof(File *) * new_nfiles));

        if (new_ofiles == nullptr) {
            return EMFILE;
        }

        memset(new_ofiles, 0, sizeof(File *) * new_nfiles);

        if (fd_ofiles != nullptr) {
            memmove(new_ofiles, fd_ofiles, sizeof(File *) * fd_nfiles);
            kxx::Heap::free(fd_ofiles);
        }

        fd_ofiles = new_ofiles;
        fd_nfiles = new_nfiles;

        // actual first free slot
        for (int i = 0; i < fd_nfiles; i++) {
            if (fd_ofiles[i] == nullptr) {
                indx = i;
                return 0;
            }
        }

        return EMFILE;
    }

    File *FileContext::remove_fd(const int indx) {
        if (indx < 0 || indx >= fd_nfiles) {
            return nullptr;
        }

        File *fp = fd_ofiles[indx];
        fd_ofiles[indx] = nullptr;
        return fp;
    }

    void FileContext::free_fd(const int indx) {
        File *fp = remove_fd(indx);
        if (fp != nullptr) {
            delete fp;
        }
    }

    File *FileContext::get_file(const int fd) const {
        if (fd < 0 || fd >= fd_nfiles) {
            return nullptr;
        }

        return fd_ofiles[fd];
    }

} // namespace VFS
