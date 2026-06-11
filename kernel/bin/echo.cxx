#include <libkxx/print.hxx>
#include <bin/echo.hxx>

namespace Shell::Echo {
    int exec(const kxx::Vector<kxx::String> &args) {
        for (size_t i = 1; i < args.get_size(); i++) {
            kxx::print(args.get(i));

            if (i + 1 != args.get_size()) {
                kxx::print(' ');
            }
        }

        kxx::println();
        return 0;
    }

    Command get_command() {
        return {
                .name = "echo",
                .exec = exec,
        };
    }
} // namespace Shell::Echo
