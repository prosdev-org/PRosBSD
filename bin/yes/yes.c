#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        for (int i = 1; i < argc; i++) {
            strcat(args, argv[i]);
            if (argc > i + 1)
                strcat(args, " ");
        }
        for (;;) {
            printf("%s\n", args);
        }

        free(args); // never reached
    } else {
        for (;;) {
            printf("y\n");
        }
    }
    return 0;
}
