#ifndef SHELL_COMMAND_HXX
#define SHELL_COMMAND_HXX

#include <libkxx/vector.hxx>

namespace Shell {
    struct Command {
        const char *name;
        int (*exec)(const kxx::Vector<kxx::String> &args);
    };
} // namespace Shell

#endif
