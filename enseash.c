#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/sysmacros.h>
#include <string.h>

#define MESSAGE "Welcome to ENSEA Tiny Shell.\nType 'exit' to quit."

void exitWithError(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (write(STDOUT_FILENO, MESSAGE, strlen(MESSAGE)) == -1) {
        exitWithError("stdout: ");
    }

    exit(EXIT_SUCCESS);
}