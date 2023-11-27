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
#define BUFFER_LEN 100

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
    char readed;
    int spaceCounter = 0;
    char word[100];
    char counter = 0;
    do{
        readed = in[counter];
        //if (readed == ' '){

        //}
    } while (readed != '\0');
}

int main(int argc, char* argv[]) {
    bool control = true;
    char buffer[BUFFER_LEN];
    char** toTerminal;
    int bufferReaded;
    int status;
    do {
        writeSTDout(TERMINAL_TAG);
        ssize_t read = readSTDin(buffer);
        if ((strcmp("exit", buffer) == 0) || read == 0){
            writeSTDout("Bye\n");
            exit(EXIT_SUCCESS);
        }else {
            //split(buffer, toTerminal);
            pid_t pid;
            pid = fork();
            if (pid > 0){
                wait(&status);
            }
            else if (pid == 0){
                if (execlp(buffer, buffer, (char*)NULL) == -1) {
                    perror("Command failed\n");
                    exit(EXIT_FAILURE);
                } else {
                    writeSTDout(TERMINAL_TAG);
                    exit(EXIT_FAILURE);
                }
            } else if(pid == -1){
                perror("Command failed\n");
                exit(EXIT_FAILURE);
            }
        }
    } while(control);
    exit(EXIT_FAILURE);
}