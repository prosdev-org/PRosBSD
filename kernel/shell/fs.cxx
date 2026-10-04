#include <errno.h>
#include <fcntl.h>
#include <libkxx/print.hxx>
#include <libkxx/string.hxx>
#include <shell/fs.hxx>
#include <stdint.h>
#include <string.h>
#include <sys/kernel.hxx>
#include <sys/stat.h>
#include <sys/uio.hxx>
#include <unique/log.hxx>
#include <unistd.h>
#include <vfs/vfs_lookup.hxx>
#include <vfs/vfs_subr.hxx>
#include <vfs/vfs_syscalls.hxx>
#include <vfs/vfs_vnode.hxx>

namespace Shell::Fs {
    kxx::StringView get_logger_prefix() {
        return "FS";
    }
    namespace {
        constexpr size_t PATH_MAXIMUM = 1024;
        kxx::String current_directory("/"); // TODO: FIX DIRTY GLOBAL 0proc HACK

        bool make_path(const kxx::String &arg, char *dst, size_t capacity) {
            if (arg.get_size() == 0) {
                return false;
            }

            size_t pos = 0;
            if (arg.get(0) != '/') {
                if (current_directory.get_size() + 1 + arg.get_size() + 1 > capacity) {
                    return false;
                }

                for (size_t i = 0; i < current_directory.get_size(); ++i) {
                    dst[pos++] = current_directory.get(i);
                }

                if (pos != 1 || dst[0] != '/') {
                    dst[pos++] = '/';
                }
            } else if (arg.get_size() + 1 > capacity) {
                return false;
            }

            for (size_t i = 0; i < arg.get_size(); ++i) {
                dst[pos++] = arg.get(i);
            }
            dst[pos] = '\0';
            return true;
        }

        int open_file(Sys::Process &p, const char *path, int mode, int cmode, int &fd) {
            VFS::open_args args{const_cast<char *>(path), mode, cmode};
            int retval = -1;
            const int error = VFS::sys_open(p, &args, &retval);

            if (error == 0) {
                fd = retval;
            }
            return error;
        }

        int close_file(Sys::Process &p, int fd) {
            struct close_call_args {
                int fd;
            };
            close_call_args args{fd};
            int retval = 0;

            return VFS::sys_close(p, &args, &retval);
        }

        int mkdir_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: mkdir PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("mkdir: path too long");
                return 1;
            }

            struct mkdir_call_args {
                char *name;
                int dmode;
            };
            mkdir_call_args uap{path, 0755};
            int retval = 0;
            const int error = VFS::sys_mkdir(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("mkdir", error);
                return 1;
            }
            return 0;
        }

        int touch_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: touch PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("touch: path too long");
                return 1;
            }

            Sys::Process &p = *Sys::Kernel::proc0_ref();
            int fd = -1;
            const int error = open_file(p, path, O_WRONLY | O_CREAT, 0644, fd);

            if (error != 0) {
                LOG("touch", error);
                return 1;
            }
            const int close_error = close_file(p, fd);
            if (close_error != 0) {
                LOG("touch", close_error);
                return 1;
            }
            return 0;
        }

        int cat_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: cat PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("cat: path too long");
                return 1;
            }

            Sys::Process &p = *Sys::Kernel::proc0_ref();
            int fd = -1;
            int error = open_file(p, path, O_RDONLY, 0, fd);
            if (error != 0) {
                LOG("cat", error);
                return 1;
            }

            char buffer[256];
            for (;;) {
                struct read_call_args {
                    int fd;
                    void *buf;
                    int count;
                };
                read_call_args uap{fd, buffer, static_cast<int>(sizeof(buffer))};
                int count = 0;
                error = VFS::sys_read(p, &uap, &count);
                if (error != 0) {
                    LOG("cat", error);
                    close_file(p, fd);
                    return 1;
                }
                if (count == 0) {
                    break;
                }
                kxx::print(kxx::String(buffer, static_cast<size_t>(count)));
            }

            error = close_file(p, fd);
            if (error != 0) {
                LOG("cat", error);
                return 1;
            }
            return 0;
        }

        int write_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() < 3) {
                kxx::println("usage: write PATH TEXT...");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("write: path too long");
                return 1;
            }

            char text[1024];
            size_t text_len = 0;
            for (size_t i = 2; i < args.get_size(); ++i) {
                if (i != 2) {
                    if (text_len >= sizeof(text) - 1) {
                        kxx::println("write: text too long");
                        return 1;
                    }
                    text[text_len++] = ' ';
                }
                if (text_len + args.get(i).get_size() >= sizeof(text)) {
                    kxx::println("write: text too long");
                    return 1;
                }
                for (size_t j = 0; j < args.get(i).get_size(); ++j) {
                    text[text_len++] = args.get(i).get(j);
                }
            }

            Sys::Process &p = *Sys::Kernel::proc0_ref();
            int fd = -1;
            int error = open_file(p, path, O_WRONLY | O_CREAT | O_TRUNC, 0644, fd);
            if (error != 0) {
                LOG("write", error);
                return 1;
            }

            struct write_call_args {
                int fd;
                const void *buf;
                int count;
            };
            write_call_args uap{fd, text, static_cast<int>(text_len)};
            int written = 0;
            error = VFS::sys_write(p, &uap, &written);
            if (error != 0 || written != static_cast<int>(text_len)) {
                LOG("write", error != 0 ? error : EIO);
                close_file(p, fd);
                return 1;
            }

            error = close_file(p, fd);
            if (error != 0) {
                LOG("write", error);
                return 1;
            }
            return 0;
        }

        int rm_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: rm PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("rm: path too long");
                return 1;
            }

            struct unlink_call_args {
                char *fname;
            };
            unlink_call_args uap{path};
            int retval = 0;
            const int error = VFS::sys_unlink(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("rm", error);
                return 1;
            }
            return 0;
        }

        int rmdir_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: rmdir PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("rmdir: path too long");
                return 1;
            }

            struct rmdir_call_args {
                char *name;
            };
            rmdir_call_args uap{path};
            int retval = 0;
            const int error = VFS::sys_rmdir(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("rmdir", error);
                return 1;
            }
            return 0;
        }

        int mv_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 3) {
                kxx::println("usage: mv FROM TO");
                return 1;
            }

            char from[PATH_MAXIMUM + 1];
            char to[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), from, sizeof(from)) ||
                !make_path(args.get(2), to, sizeof(to))) {
                kxx::println("mv: path too long");
                return 1;
            }

            struct rename_call_args {
                char *from;
                char *to;
            };
            rename_call_args uap{from, to};
            int retval = 0;
            const int error = VFS::sys_rename(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("mv", error);
                return 1;
            }
            return 0;
        }

        int cd_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: cd PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("cd: path too long");
                return 1;
            }

            struct chdir_call_args {
                char *fname;
            };
            chdir_call_args uap{path};
            int retval = 0;
            const int error = VFS::sys_chdir(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("cd", error);
                return 1;
            }

            const kxx::StringView arg(args.get(1));
            if (arg.get_size() > 0 && arg.get(0) == '/') {
                current_directory = args.get(1);
                return 0;
            }

            if (arg == kxx::StringView(".")) {
                return 0;
            }

            if (arg == kxx::StringView("..")) {
                if (kxx::StringView(current_directory) != kxx::StringView("/")) {
                    size_t end = current_directory.get_size();
                    while (end > 1 && current_directory.get(end - 1) != '/') {
                        --end;
                    }
                    while (end > 1 && current_directory.get(end - 1) == '/') {
                        --end;
                    }
                    kxx::String parent;
                    for (size_t i = 0; i < end; ++i) {
                        parent.push_back(current_directory.get(i));
                    }
                    if (parent.get_size() == 0) {
                        parent = kxx::String("/");
                    }
                    current_directory = parent;
                }
                return 0;
            }

            kxx::String next;
            if (kxx::StringView(current_directory) == kxx::StringView("/")) {
                next.push_back('/');
            } else {
                for (size_t i = 0; i < current_directory.get_size(); ++i) {
                    next.push_back(current_directory.get(i));
                }
                next.push_back('/');
            }
            for (size_t i = 0; i < arg.get_size(); ++i) {
                next.push_back(arg.get(i));
            }
            current_directory = next;
            return 0;
        }

        int pwd_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 1) {
                kxx::println("usage: pwd");
                return 1;
            }
            kxx::println(current_directory);
            return 0;
        }

        struct Dirent {
            ino_t d_ino;
            uint16_t d_reclen;
            uint8_t d_type;
            uint8_t d_namlen;
        };

        int ls_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() > 2) {
                kxx::println("usage: ls PATH");
                return 1;
            }

            kxx::String arg = args.get_size() == 2 ? args.get(1) : kxx::String(".");
            char path[PATH_MAXIMUM + 1];
            if (!make_path(arg, path, sizeof(path))) {
                kxx::println("ls: path too long");
                return 1;
            }
            Sys::Process &p = *Sys::Kernel::proc0_ref();

            int fd = -1;
            int error = open_file(p, path, O_RDONLY, 0, fd);
            if (error != 0) {
                LOG("ls", error);
                return 1;
            }

            char buffer[4096];
            long base = 0;

            for (;;) {
                struct getdirentries_call_args {
                    int fd;
                    char *buf;
                    unsigned int count;
                    long *basep;
                };
                getdirentries_call_args uap{
                        fd,
                        buffer,
                        static_cast<unsigned int>(sizeof(buffer)),
                        &base};

                int nread = 0;
                error = VFS::sys_getdirentries(p, &uap, &nread);
                if (error != 0) {
                    LOG("ls", error);
                    close_file(p, fd);
                    return 1;
                }
                if (nread == 0) {
                    break;
                }

                int pos = 0;
                while (pos < nread) {
                    if (nread - pos < static_cast<int>(sizeof(Dirent))) {
                        close_file(p, fd);
                        kxx::println("ls: strange entry");
                        return 1;
                    }

                    auto *de = reinterpret_cast<const Dirent *>(buffer + pos);

                    if (de->d_reclen < sizeof(Dirent) ||
                        de->d_reclen > nread - pos ||
                        de->d_namlen > de->d_reclen - sizeof(Dirent)) {
                        close_file(p, fd);
                        kxx::println("ls: strange entry");
                        return 1;
                    }

                    kxx::println(kxx::String(
                            buffer + pos + sizeof(Dirent),
                            de->d_namlen));

                    pos += de->d_reclen;
                }
            }
            error = close_file(p, fd);
            if (error != 0) {
                LOG("ls", error);
                return 1;
            }

            return 0;
        }

        int stat_exec(const kxx::Vector<kxx::String> &args) {
            if (args.get_size() != 2) {
                kxx::println("usage: stat PATH");
                return 1;
            }

            char path[PATH_MAXIMUM + 1];
            if (!make_path(args.get(1), path, sizeof(path))) {
                kxx::println("stat: path too long");
                return 1;
            }

            struct stat_call_args {
                char *fname;
                struct stat *ub;
            };
            struct stat sb{};
            stat_call_args uap{path, &sb};
            int retval = 0;
            const int error = VFS::sys_stat(*Sys::Kernel::proc0_ref(), &uap, &retval);
            if (error != 0) {
                LOG("stat", error);
                return 1;
            }
            kxx::println("ino=", sb.st_ino,
                         " mode=", sb.st_mode,
                         " nlink=", sb.st_nlink,
                         " size=", sb.st_size);
            return 0;
        }
    } // namespace

    Command get_mkdir_command() {
        return {"mkdir", mkdir_exec};
    }
    Command get_touch_command() {
        return {"touch", touch_exec};
    }
    Command get_cat_command() {
        return {"cat", cat_exec};
    }
    Command get_write_command() {
        return {"write", write_exec};
    }
    Command get_rm_command() {
        return {"rm", rm_exec};
    }
    Command get_rmdir_command() {
        return {"rmdir", rmdir_exec};
    }
    Command get_mv_command() {
        return {"mv", mv_exec};
    }
    Command get_cd_command() {
        return {"cd", cd_exec};
    }
    Command get_pwd_command() {
        return {"pwd", pwd_exec};
    }
    Command get_ls_command() {
        return {"ls", ls_exec};
    }
    Command get_stat_command() {
        return {"stat", stat_exec};
    }
} // namespace Shell::Fs
