#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 20480
bool real_file = false;
int buffer_size = BUFFER;
char buffer[BUFFER];
ssize_t readed;
int error = 0;
char **filenames = NULL;
int files = 0;

void cat(const char *filename) {
    real_file = true;
    int fd;
    if (strcmp(filename, "-") == 0) {
        fd = 0;
    } else {
        fd = open(filename, O_RDONLY);
    }
    if (fd < 0) {
        fprintf(stderr, "Unable to open: %s (%s)!\n", filename, strerror(errno));
        error = 1;
        return;
    }

    while ((readed = read(fd, buffer, buffer_size)) > 0) {
        ssize_t written = 0;
        while (written < readed) {
            const ssize_t res = write(1, buffer + written, readed - written);
            if (res < 0) {
                fprintf(stderr, "Cannot write to STDOUT (%s)!\n", strerror(errno));
                error = 1;
                if (strcmp(filename, "-") != 0)
                    close(fd);
                return;
            }
            written += res;
        }
    }
    if (strcmp(filename, "-") != 0)
        close(fd);
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0)
            buffer_size = 1;
        else if (argv[i][1] != '\0' && argv[i][1] != '-' && argv[i][0] == '-') { // check for undefined flag
            fprintf(stderr, "Undefined flag: %s!\n", argv[i]);
            free(filenames);
            return 1;
        } else {
            char **alloced = realloc(filenames, (files + 1) * sizeof(char *));
            if (alloced == NULL) {
                fprintf(stderr, "Unable to allocate memory (%s!)\n",
                        strerror(errno));
                free(filenames);
                return 1;
            }
            filenames = alloced;
            filenames[files] = argv[i];
            files++;
        }
    }

    if (files == 0) {
        cat("-");
    } else {
        for (int i = 0; i < files; i++) {
            cat(filenames[i]);
        }
    }

    free(filenames);
    return error;
}
