#include <libkxx/print.hxx>
#include <machine/init.hxx>
#include <sys/kernel.hxx>
#include <unique/extern_c.h>

EXTERN_C NORETURN void Kernel_entry() {
    Machine::init();
}

namespace Sys::Kernel {
    static OutputStream *output_stream;

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

    void set_output_stream(OutputStream *new_output_stream) {
        delete output_stream;
        output_stream = new_output_stream;
    }

    OutputStream *get_output_stream() {
        return output_stream;
    }
} // namespace Sys::Kernel
