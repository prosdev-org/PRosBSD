#include <libkxx/print.hxx>
#include <shell/uname.hxx>

namespace Shell::Uname {
    int exec(const kxx::Vector<kxx::String> &args) {
        if (args.get_size() != 1) {
            kxx::println("usage: uname");
            return 1;
        }

        kxx::println("PRosBSD");
        return 0;
    }

    Command get_command() {
        return {
                .name = "uname",
                .exec = exec,
        };
    }
} // namespace Shell::Uname
