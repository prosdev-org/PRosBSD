#include <libkxx/print.hxx>
#include <libkxx/unique_ptr.hxx>
#include <machine/init.hxx>
#include <sys/dummy_output_stream.hxx>
#include <sys/kernel.hxx>
#include <unique/extern_c.h>

namespace Sys::Kernel {
    struct Storage {
        kxx::UniquePtr<OutputStream> output_stream;
    };

    static kxx::UniquePtr<OutputStream> &(*output_stream)();

    EXTERN_C NORETURN void Kernel_entry() {
        static Storage storage = {
                kxx::UniquePtr(static_cast<OutputStream *>(new DummyOutputStream)),
        };

        output_stream = []() -> kxx::UniquePtr<OutputStream> & {
            return storage.output_stream;
        };

        Machine::init();
    }

    void main() {
        // Temp demo
        kxx::println("Welcome to PRosBSD v." PROSBSD_VERSION " !");
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

        for (;;)
            ;
    }

    void set_output_stream(kxx::UniquePtr<OutputStream> &&new_output_stream) {
        output_stream() = kxx::move(new_output_stream);
    }

    OutputStream &get_output_stream() {
        return *output_stream();
    }
} // namespace Sys::Kernel
