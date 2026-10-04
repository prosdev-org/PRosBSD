#include <libkxx/heap.hxx>
#include <libkxx/print.hxx>
#include <libkxx/shared_ptr.hxx>
#include <libkxx/unique_ptr.hxx>
#include <machine/cpu.hxx>
#include <machine/init.hxx>
#include <shell/shell.hxx>
#include <sys/dummy_input_stream.hxx>
#include <sys/dummy_output_stream.hxx>
#include <sys/dummy_timer.hxx>
#include <sys/kernel.hxx>
#include <sys/log.hxx>
#include <sys/process.hxx>
#include <unique/extern_c.h>
#include <unique/log.hxx>
#include <vfs/ramfs.hxx>
#include <vfs/vfs_subr.hxx>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vfs/file.hxx>
#include <vfs/vfs_syscalls.hxx>

namespace Sys::Kernel {
    struct Storage {
        kxx::UniquePtr<Process> proc0;
        kxx::UniquePtr<OutputStream> output_stream;
        kxx::UniquePtr<InputStream> input_stream;
        kxx::UniquePtr<Timer> timer;
    };

    static kxx::UniquePtr<Process> &(*proc0)();
    static kxx::UniquePtr<OutputStream> &(*output_stream)();
    static kxx::UniquePtr<InputStream> &(*input_stream)();
    static kxx::UniquePtr<Timer> &(*timer)();

    void setup_storage() {
        static Storage storage = {
                .proc0 = kxx::UniquePtr<Process>(nullptr),
                .output_stream = kxx::UniquePtr(static_cast<OutputStream *>(
                        new DummyOutputStream)),
                .input_stream = kxx::UniquePtr(static_cast<InputStream *>(
                        new DummyInputStream)),
                .timer = kxx::UniquePtr(static_cast<Timer *>(
                        new DummyTimer)),
        };

        proc0 = []() -> kxx::UniquePtr<Process> & {
            return storage.proc0;
        };

        output_stream = []() -> kxx::UniquePtr<OutputStream> & {
            return storage.output_stream;
        };

        input_stream = []() -> kxx::UniquePtr<InputStream> & {
            return storage.input_stream;
        };

        timer = []() -> kxx::UniquePtr<Timer> & {
            return storage.timer;
        };
    }

    // AI WRITTEN TEST. TODO: REMOVE, WHEN VFS GONNA BE IN MAIN BRANCH
    static void test_vfs() {
        log("=== VFS/RamFS smoke test ===");

        Sys::Process &p = *proc0_ref();
        int retval = 0;

        // --- mkdir /testdir ---
        {
            struct mkdir_args {
                char *name;
                int dmode;
            };
            static char path[] = "/testdir";
            mkdir_args args{path, 0755};
            const int error = VFS::sys_mkdir(p, &args, &retval);
            log("mkdir /testdir -> error=", error);
        }

        // --- open(O_RDWR | O_CREAT) /testdir/hello.txt ---
        int fd = -1;
        {
            static char fname[] = "/testdir/hello.txt";
            VFS::open_args args{fname, O_RDWR | O_CREAT, 0644};
            const int error = VFS::sys_open(p, &args, &fd);
            log("open /testdir/hello.txt -> error=", error, ", fd=", fd);
        }

        if (fd < 0) {
            log("=== VFS test aborted (open failed) ===");
            return;
        }

        // --- write ---
        {
            static const char text[] = "Hello, RamFS!";
            struct write_args {
                int fd;
                const void *buf;
                int count;
            };
            write_args args{fd, text, static_cast<int>(sizeof(text) - 1)};
            int wret = 0;
            const int error = VFS::sys_write(p, &args, &wret);
            log("write -> error=", error, ", n=", wret);
        }

        // --- lseek(fd, 0, L_SET) ---
        {
            struct lseek_args {
                int fdes;
                off_t off;
                int sbase;
            };
            lseek_args args{fd, 0, L_SET};
            int lret = 0;
            const int error = VFS::sys_lseek(p, &args, &lret);
            log("lseek -> error=", error, ", off=", lret);
        }

        // --- read back ---
        {
            char buf[64] = {};
            struct read_args {
                int fd;
                void *buf;
                int count;
            };
            read_args args{fd, buf, static_cast<int>(sizeof(buf) - 1)};
            int rret = 0;
            const int error = VFS::sys_read(p, &args, &rret);
            log("read -> error=", error, ", n=", rret,
                ", data=\"", kxx::StringView(buf));
        }

        // --- fstat ---
        {
            struct fstat_args {
                int fd;
                struct stat *sb;
            };
            struct stat sb{};
            fstat_args args{fd, &sb};
            const int error = VFS::sys_fstat(p, &args, &retval);
            log("fstat -> error=", error, ", size=", sb.st_size);
        }

        // --- close ---
        {
            struct close_args {
                int fd;
            };
            close_args args{fd};
            const int error = VFS::sys_close(p, &args, &retval);
            log("close -> error=", error);
        }

        // --- unlink ---
        {
            struct unlink_args {
                char *fname;
            };
            static char fname[] = "/testdir/hello.txt";
            unlink_args args{fname};
            const int error = VFS::sys_unlink(p, &args, &retval);
            log("unlink -> error=", error);
        }

        // --- rmdir ---
        {
            struct rmdir_args {
                char *name;
            };
            static char path[] = "/testdir";
            rmdir_args args{path};
            const int error = VFS::sys_rmdir(p, &args, &retval);
            log("rmdir -> error=", error);
        }

        log("=== End of VFS/RamFS smoke test ===\n");
    }

    EXTERN_C NORETURN void Kernel_entry() {
        setup_storage();
        Machine::init();
    }

    kxx::StringView get_logger_prefix() {
        return "sys/kernel";
    }

    void main() {
        proc0() = kxx::UniquePtr(new Process(Process::init_proc0()));

        VFS::RamFS::init();
        VFS::vfs_init();
        Shell::init();

        kxx::Heap::log_info();

        log("Welcome to PRosBSD v." PROSBSD_VERSION " !\n"
            "Copyright (c) 2025-2026 PRosDev.org. All rights reserved.\n"
            "Copyright (c) 1982, 1983, 1985, 1986, 1989, 1990, 1991\n"
            "The Regents of the University of California. All rights reserved.\n"
            "\n"
            "PRosBSD is distributed under the BSD 3-Clause license.\n"
            "\n"
            "This product includes software developed by the University of\n"
            "California, Berkeley and its contributors.\n");

        test_vfs();

        Shell::loop();

        for (;;) {
            Machine::Cpu::relax();
        }
    }

    kxx::UniquePtr<Process> &proc0_ref() {
        return proc0();
    }

    void set_output_stream(kxx::UniquePtr<OutputStream> &&new_output_stream) {
        output_stream() = kxx::move(new_output_stream);
        LOG("set new OutputStream (",
            reinterpret_cast<void *>(output_stream().get_raw()),
            ")");
    }

    OutputStream &get_output_stream() {
        return *output_stream();
    }

    void set_input_stream(kxx::UniquePtr<InputStream> &&new_input_stream) {
        input_stream() = kxx::move(new_input_stream);
        LOG("set new InputStream (",
            reinterpret_cast<void *>(input_stream().get_raw()),
            ")");
    }

    InputStream &get_input_stream() {
        return *input_stream();
    }

    void set_timer(kxx::UniquePtr<Timer> &&new_timer) {
        timer() = kxx::move(new_timer);
        LOG("set new Timer (",
            reinterpret_cast<void *>(timer().get_raw()),
            ")");
    }

    Timer &get_timer() {
        return *timer();
    }
} // namespace Sys::Kernel
