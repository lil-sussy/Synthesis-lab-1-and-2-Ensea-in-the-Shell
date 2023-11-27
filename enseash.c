#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h> 

#define MESSAGE "Welcome to ENSEA Tiny Shell.\nType 'exit' to quit.\n"
#define TERMINAL_TAG "enseash % "

void exitWithError(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void writeSTDout(char* message) {
    if (write(STDOUT_FILENO, message, strlen(message)) == -1) {
        exitWithError("stdout: ");
    }
}

int main(int argc, char* argv[]) {
    bool control = true;
    writeSTDout(MESSAGE);
    do {
        writeSTDout(TERMINAL_TAG);
        control = false;
    } while(control);
    exit(EXIT_SUCCESS);
}