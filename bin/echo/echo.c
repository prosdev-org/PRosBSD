#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    size_t argsc = 0;
    for (int i = 1; i < argc; i++) {
        argsc += strlen(argv[i]) + 1; // terminator
    }

    char *args = malloc(argsc + 1);
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

    printf("%s\n", args);

    free(args);
    return 0;
}
