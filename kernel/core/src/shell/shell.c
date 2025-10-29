#include <shell/clear.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char *name;
    int (*function)(int argc, const char **argv);
} command_t;

static command_t commands[] = {
    {"clear", shell_clear}
};

static int exit_code = 0;

static int exec(const int argc, const char **argv) {
    if (!argc) {
        return 0;
    }

    for (size_t i = 0; i < sizeof(commands) / sizeof(*commands); i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            return commands[i].function(argc, argv);
        }
    }

    printf("%s: command not found\n", *argv);
    return 1;
}

_Noreturn void shell_loop() {
    for (;;) {
        if (exit_code) {
            printf("\033[1;31m~$\033[0m ");
        } else {
            printf("\033[1;32m~$\033[0m ");
        }

        char str[128];
        gets_s(str, sizeof(str) / sizeof(*str));

        const char *argv[32];
        int argc = 0;

        const char *token = strtok(str, " ");
        while (argc < (int) (sizeof(argv) / sizeof(*argv) - 1) && token) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        while (token) {
            token = strtok(NULL, " ");
        }

        exit_code = exec(argc, argv);
    }
}
