#include <libkxx/input.hxx>
#include <libkxx/print.hxx>
#include <libkxx/unique_ptr.hxx>
#include <machine/cpu.hxx>
#include <machine/init.hxx>
#include <sys/dummy_input_stream.hxx>
#include <sys/dummy_output_stream.hxx>
#include <sys/dummy_timer.hxx>
#include <sys/kernel.hxx>
#include <sys/log.hxx>
#include <unique/extern_c.h>

namespace Sys::Kernel {
    struct Storage {
        kxx::UniquePtr<OutputStream> output_stream;
        kxx::UniquePtr<InputStream> input_stream;
        kxx::UniquePtr<Timer> timer;
    };

    static kxx::UniquePtr<OutputStream> &(*output_stream)();
    static kxx::UniquePtr<InputStream> &(*input_stream)();
    static kxx::UniquePtr<Timer> &(*timer)();

    void setup_storage() {
        static Storage storage = {
                .output_stream = kxx::UniquePtr(static_cast<OutputStream *>(
                        new DummyOutputStream)),
                .input_stream = kxx::UniquePtr(static_cast<InputStream *>(
                        new DummyInputStream)),
                .timer = kxx::UniquePtr(static_cast<Timer *>(
                        new DummyTimer)),
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

    void main() {
        // Temp demo
        log("Welcome to PRosBSD v." PROSBSD_VERSION " !");
        kxx::println("             ,        ,\n"
                     "            /(        )`\n"
                     "            \\ \\___   / |\n"
                     "            /- _  `-/  '\n"
                     "           (/\\/ \\ \\   /\\\n"
                     "           / /   | `    \\\n"
                     "           O O   ) /    |\n"
                     "           `-^--'`<     '\n"
                     "          (_.)  _  )   /\n"
                     "           `.___/`    /\n"
                     "             `-----' /\n"
                     "<----.     __ / __   \\\n"
                     "<----|====O)))==) \\) /====|\n"
                     "<----'    `--' `.__,' \\\n"
                     "             |        |\n"
                     "              \\       /       /\\\n"
                     "         ______( (_  / \\______/\n"
                     "       ,'  ,-----'   |\n"
                     "       `--{__________)\n");

        for (;;) {
            kxx::print(kxx::input());
        }
    }

    void set_output_stream(kxx::UniquePtr<OutputStream> &&new_output_stream) {
        output_stream() = kxx::move(new_output_stream);
    }

    OutputStream &get_output_stream() {
        return *output_stream();
    }

    void set_input_stream(kxx::UniquePtr<InputStream> &&new_input_stream) {
        input_stream() = kxx::move(new_input_stream);
    }

    InputStream &get_input_stream() {
        return *input_stream();
    }

    void set_timer(kxx::UniquePtr<Timer> &&new_timer) {
        timer() = kxx::move(new_timer);
    }

    Timer &get_timer() {
        return *timer();
    }
} // namespace Sys::Kernel
