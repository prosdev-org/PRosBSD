#include <ctype.h>
#include <libkxx/input.hxx>
#include <libkxx/print.hxx>
#include <libkxx/string.hxx>
#include <shell/command.hxx>
#include <shell/echo.hxx>
#include <shell/shell.hxx>
#include <shell/uname.hxx>

namespace Shell {
    struct Storage {
        kxx::Vector<Command> commands;
    };

    static kxx::Vector<Command> &(*commands)();

    void setup_storage() {
        static Storage storage;

        commands = []() -> kxx::Vector<Command> & {
            return storage.commands;
        };
    }

    void push_non_empty_current_string(
            kxx::String &current_string,
            kxx::Vector<kxx::String> &result) {
        if (current_string.get_size() != 0) {
            result.push_back(current_string);
            const kxx::String new_string;
            current_string = new_string;
        }
    }

    kxx::Vector<kxx::String> parse(const kxx::StringView string_view) {
        kxx::Vector<kxx::String> result;
        kxx::String current_string;
        bool in_spacing = false;

        for (size_t i = 0; i < string_view.get_size(); i++) {
            if (isspace(string_view.get(i))) {
                if (in_spacing) {
                    continue;
                }

                push_non_empty_current_string(current_string, result);

                in_spacing = true;
                continue;
            }

            in_spacing = false;

            current_string.push_back(string_view.get(i));
        }

        push_non_empty_current_string(current_string, result);
        return result;
    }

    int exec(const kxx::Vector<kxx::String> &args) {
        ASSERT(args.get_size() > 0);

        for (size_t i = 0; i < commands().get_size(); i++) {
            if (kxx::StringView(args.get(0)) == commands().get(i).name) {
                return commands().get(i).exec(args);
            }
        }

        kxx::println(args.get(0), ": command not found");
        return 1;
    }

    void prompt() {
        kxx::print("$ ");

        const kxx::String string = kxx::input();
        const kxx::Vector<kxx::String> args = parse(string);
        if (args.get_size() == 0) {
            return;
        }

        exec(args);
    }

    void init() {
        setup_storage();
        commands().push_back(Echo::get_command());
        commands().push_back(Uname::get_command());
    }

    void loop() {
        for (;;) {
            prompt();
        }
    }
} // namespace Shell
