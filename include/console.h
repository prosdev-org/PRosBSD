#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

// Console functions
void console_init(void);
void console_prompt(void);
void console_process_input(void);
void console_print_error(const char *command);

// Command functions
void cmd_info(void);
void cmd_help(void);

#endif // CONSOLE_H 