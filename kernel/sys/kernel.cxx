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

    EXTERN_C NORETURN void Kernel_entry() {
        setup_storage();
        Machine::init();
    }

    kxx::StringView get_logger_prefix() {
        return "sys/kernel";
    }

    void main() {
        proc0() = kxx::UniquePtr(new Process(Process::init_proc0()));

        Shell::init();

        kxx::Heap::log_info();

        log("Welcome to PRosBSD v." PROSBSD_VERSION " !\n"
            "Copyright (c) 2025-2026 PRosDev.org. All rights reserved.\n"
            "Copyright (c) 1986, 1989, 1991\n"
            "The Regents of the University of California. All rights reserved.\n"
            "\n"
            "PRosBSD is distributed under the BSD 3-Clause license.\n"
            "\n"
            "This product includes software developed by the University of\n"
            "California, Berkeley and its contributors.\n");

        Shell::loop();

        for (;;) {
            Machine::Cpu::relax();
        }
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
