#include <console.h>
#include <extrns.h>
#include <keyboard.h>
#include <stdint.h>
#include <string.h>
#include <tty.h>
#include <generated/version.h>

// Color attributes (copied from tty.h for compatibility)
#define VGA_LIGHT_GREEN   0x0A
#define VGA_LIGHT_RED     0x0C
#define VGA_LIGHT_CYAN    0x0B
#define VGA_LIGHT_WHITE   0x0F

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

static char command_buffer[MAX_COMMAND_LENGTH];
static int command_pos = 0;
static int console_active = 0;

void console_init(void) {
    command_pos = 0;
    console_active = 1;
    console_prompt();
}

void console_prompt(void) {
    if (console_active) {
        // Print prompt without newline
        putck_colored('$', VGA_LIGHT_GREEN);
        putck_colored(' ', VGA_LIGHT_GREEN);
    }
}

void console_print_error(const char *command) {
    // Print command name
    while (*command) {
        putck_colored(*command++, VGA_LIGHT_RED);
    }
    // Print ": not found"
    const char *error_msg = ": not found";
    while (*error_msg) {
        putck_colored(*error_msg++, VGA_LIGHT_RED);
    }
    putck_colored('\n', VGA_LIGHT_RED);
}

void cmd_info(void) {
    // Print system name
    const char *name = "PRos32 Operating System";
    while (*name) {
        putck_colored(*name++, VGA_LIGHT_CYAN);
    }
    putck_colored('\n', VGA_LIGHT_CYAN);
    
    // Print version dynamically
    putck_colored('V', VGA_LIGHT_CYAN);
    putck_colored('e', VGA_LIGHT_CYAN);
    putck_colored('r', VGA_LIGHT_CYAN);
    putck_colored('s', VGA_LIGHT_CYAN);
    putck_colored('i', VGA_LIGHT_CYAN);
    putck_colored('o', VGA_LIGHT_CYAN);
    putck_colored('n', VGA_LIGHT_CYAN);
    putck_colored(':', VGA_LIGHT_CYAN);
    putck_colored(' ', VGA_LIGHT_CYAN);
    
    // Print version string
    const char *version = VERSION_STRING;
    while (*version) {
        putck_colored(*version++, VGA_LIGHT_CYAN);
    }
    putck_colored('\n', VGA_LIGHT_CYAN);
    
    // Print other info
    const char *lines[] = {
        "Architecture: x86 (32-bit)",
        "Kernel: Monolithic",
        "Filesystem: FAT32",
        "License: MIT"
    };
    
    for (int i = 0; i < 4; i++) {
        const char *line = lines[i];
        while (*line) {
            putck_colored(*line++, VGA_LIGHT_CYAN);
        }
        putck_colored('\n', VGA_LIGHT_CYAN);
    }
}

void cmd_help(void) {
    const char *lines[] = {
        "Available commands:",
        "  info  - Show system information",
        "  help  - Show this help message",
        "  clear - Clear the screen"
    };
    
    for (int i = 0; i < 4; i++) {
        const char *line = lines[i];
        uint8_t color = (i == 0) ? VGA_LIGHT_CYAN : VGA_LIGHT_WHITE;
        while (*line) {
            putck_colored(*line++, color);
        }
        putck_colored('\n', color);
    }
}

void console_process_input(void) {
    char c = getchar();
    
    // Handle backspace
    if (c == '\b') {
        if (command_pos > 0) {
            command_pos--;
            putck('\b');
            putck(' ');
            putck('\b');
        }
        return;
    }
    
    // Handle enter key
    if (c == '\n' || c == '\r') {
        putck('\n');
        
        // Null-terminate the command
        command_buffer[command_pos] = '\0';
        
        // Process the command
        if (command_pos > 0) {
            if (strcmp(command_buffer, "info") == 0) {
                cmd_info();
            } else if (strcmp(command_buffer, "help") == 0) {
                cmd_help();
            } else if (strcmp(command_buffer, "clear") == 0) {
                cleark();
            } else {
                console_print_error(command_buffer);
            }
        }
        
        // Reset command buffer
        command_pos = 0;
        console_prompt();
        return;
    }
    
    // Handle printable characters
    if (c >= ' ' && c <= '~' && command_pos < MAX_COMMAND_LENGTH - 1) {
        command_buffer[command_pos++] = c;
        putck(c);
    }
}

void console_set_active(int active) {
    console_active = active;
} 