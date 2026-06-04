#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    size_t argsc = 0;
    for (int i = 1; i < argc; i++) {
        argsc += strlen(argv[i]) + 1; // terminator
    }
    if (argsc > 1) {
        char *args = malloc(argsc);
        if (args == NULL) {
            fprintf(stderr, "Unable to allocate memory (%s!)\n",
                    strerror(errno));
            return 1;
        }

        args[0] = '\0';
        char *ptr = args;
        for (int i = 1; i < argc; i++) {
            size_t len = strlen(argv[i]);
            memcpy(ptr, argv[i], len);
            ptr += len;
            if (argc > i + 1)
                *ptr++ += ' ';
        }
        *ptr = '\n';
        for (;;) {
            if (!write(1, args, argsc))
                return 0; // closed
        }

        free(args); // never reached
    } else {
        for (;;) {
            if (!write(1, "y\n", 2))
                return 0; // closed
        }
    }
    return 0;
}
