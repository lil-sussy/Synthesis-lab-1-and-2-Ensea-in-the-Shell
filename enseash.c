#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>
#include <sys/wait.h>

#define MESSAGE "Welcome to ENSEA Tiny Shell.\nType 'exit' to quit.\n"
#define TERMINAL_TAG "enseash % "
#define BUFFER_LEN 1000
#define TOKEN_LEN 100
#define INT2POINTER(a) ((char*)(intptr_t)(a))

void exitWithError(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void writeSTDout(char* message) {
    if (write(STDOUT_FILENO, message, strlen(message)) == -1) {
        exitWithError("stdout: ");
    }
}

ssize_t readSTDin(char* buffer) {
    ssize_t readed;
    if ((readed = read(STDIN_FILENO, buffer, BUFFER_LEN)) == -1) {
        exitWithError("stdin: ");
    }
    buffer[readed-1] = '\0';
    return readed;
}

void split(char* in, char** out){
    char delimiter[] = " ";
    char* tokenPointer;
    int counter = 0;
    char* inCopy = malloc(100);
    strcpy(inCopy, in);

    tokenPointer = strtok(inCopy, delimiter);
    while(tokenPointer != NULL) {
        out[counter] = tokenPointer;
        counter++;
        tokenPointer = strtok(NULL, delimiter);
    }
    out[counter] = NULL;
}

void displayStatus(int status, struct timespec start, struct timespec end) {
    double deltaTimeMillis;
    deltaTimeMillis = (end.tv_sec - start.tv_sec) * 1000.0;
    deltaTimeMillis += (end.tv_nsec - start.tv_nsec) / 1000000.0;
    if (WIFEXITED(status)) {
        char toprint[100];
        sprintf(toprint, "enseash [exit:%d|%.2fms] %% ", WEXITSTATUS(status), deltaTimeMillis);
        writeSTDout(toprint);
    } else if (WIFSIGNALED(status)) {
        char toprint[100];
        sprintf(toprint, "enseash [sign:%d|%.2fms] %% ", WTERMSIG(status), deltaTimeMillis);
        writeSTDout(toprint);
    }
}

int main(int argc, char* argv[]) {
    struct timespec start, end;
    char buffer[BUFFER_LEN];                        //the buffer of the mainloop
    pid_t pid;
    int status;                                     //status for the wait
    char** tokens[TOKEN_LEN];

    writeSTDout("enseash % ");
    while(1) {
        ssize_t read = readSTDin(buffer);
        split(buffer, tokens);
        if ((strcmp("exit", buffer) == 0) || read == 0){
            writeSTDout("Bye\n");
            exit(EXIT_SUCCESS);
            break;
        }
        pid = fork();
        switch(pid) {
            case -1:  // Fork fail
                perror("Couldn't fork");
                exit(EXIT_FAILURE);
                break;
            case 0:  // Fork Success
                execvp(tokens[0], tokens);
                writeSTDout("Command not found\n");
                exit(EXIT_FAILURE);  // execlp only returns on failure
                break;
            default:
                clock_gettime(CLOCK_MONOTONIC, &start);
                waitpid(pid, &status, 0);
                clock_gettime(CLOCK_MONOTONIC, &end);
                displayStatus(status, start, end);
                break;
        }
    }
    return EXIT_FAILURE;
}
