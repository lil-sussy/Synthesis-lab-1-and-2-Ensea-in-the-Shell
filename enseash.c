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
#define SYSCALL_FAIL -1
#define FORK_PID_CHILD 0

void exitWithError(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void writeSTDout(char* message) {
    if (write(STDOUT_FILENO, message, strlen(message)) == SYSCALL_FAIL) {
        exitWithError("stdout: ");
    }
}

ssize_t readSTDin(char* buffer) {
    ssize_t readed;
    if ((readed = read(STDIN_FILENO, buffer, BUFFER_LEN)) == SYSCALL_FAIL) {
        exitWithError("stdin: ");
    }
    buffer[readed-1] = '\0';

    return readed;
}

void split(char* in, char* out[]) {
    char delimiter[] = " ";
    char* tokenPointer;
    int counter = 0;
    char* inCopy = malloc(BUFFER_LEN);
    strcpy(inCopy, in);

    //here we separete the input in tokens that we put in out
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
    char toprint[BUFFER_LEN];

    deltaTimeMillis = (end.tv_sec - start.tv_sec) * 1000.0;
    deltaTimeMillis += (end.tv_nsec - start.tv_nsec) / 1000000.0;

    //here we search if it exited or signaled something, and print it
    if (WIFEXITED(status)) {
        sprintf(toprint, "enseash [exit:%d|%.2fms] %% ", WEXITSTATUS(status), deltaTimeMillis);
        writeSTDout(toprint);

    } else if (WIFSIGNALED(status)) {
        sprintf(toprint, "enseash [sign:%d|%.2fms] %% ", WTERMSIG(status), deltaTimeMillis);
        writeSTDout(toprint);
    }
}

void guideToActions(pid_t pid, char** tokens) {
    struct timespec start, end;
    int isError;
    int status;

    switch(pid) {
        case SYSCALL_FAIL:
            exitWithError("Fork: ");
        break;

        case FORK_PID_CHILD:
            if(execvp(tokens[0], tokens) == SYSCALL_FAIL){
                writeSTDout("Command not found\n");
                exit(EXIT_FAILURE);
            }
        break;

        default:
            if(clock_gettime(CLOCK_MONOTONIC, &start) == SYSCALL_FAIL){
                exitWithError("clock start: ");
            }

            waitpid(pid, &status, 0);

            if(clock_gettime(CLOCK_MONOTONIC, &end) == SYSCALL_FAIL){
                exitWithError("clock end: ");
            }

            displayStatus(status, start, end);
        break;
    }
}

int main(int argc, char* argv[]) {
    char buffer[BUFFER_LEN];                        
    pid_t pid;                                   
    char* tokens[TOKEN_LEN];

    writeSTDout("enseash % ");

    while(true) {
        ssize_t read = readSTDin(buffer);
        split(buffer, tokens);

        if ((strcmp("exit", buffer) == 0) || read == 0){
            writeSTDout("Bye\n");
            exit(EXIT_SUCCESS);
            break;
        }

        pid = fork();
        guideToActions(pid,tokens);
    }
    return EXIT_FAILURE;
}
