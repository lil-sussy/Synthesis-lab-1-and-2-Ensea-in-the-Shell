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
#define TOKEN_LEN 20
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

void split(char* in, char out[TOKEN_LEN][BUFFER_LEN]){
    char delimiter[] = " ";
    char* tokenPointer;
    int counter = 0;

    tokenPointer = strtok(in,delimiter);
    while(tokenPointer != NULL){
        strcpy(out[counter], tokenPointer);
        counter++;
        tokenPointer = strtok(NULL,delimiter);
    }

}

int main(int argc, char* argv[]) {

    //Variables
    bool control = true;            //control of the mainloop
    char buffer[BUFFER_LEN];        //the buffer of the mainloop
    char** toTerminal;              //the expresion that goes to terminal
    int status;                     //status for the wait
    int exit;                       //the value of the return of signal
    bool isReturn;                  //1 if return, 0 if signal

    //mainloop
    do {
        //write the tag and wait for input
        writeSTDout(TERMINAL_TAG);
        ssize_t read = readSTDin(buffer);

        //compare if it is one especial case, to exit as an example.
        if ((strcmp("exit", buffer) == 0) || read == 0){
            writeSTDout("Bye\n");
            exit(EXIT_SUCCESS);
        }else {
            //if not, it execute the command
            split(buffer, toTerminal);          //split in tokens
            //the fork for the exec
            pid_t pid;
            pid = fork();

            //if is the parent, it will wait and read the exit status
            //of the child when it dies.
            if (pid > 0){
                wait(&status);
                if(WIFEXITED(status)) {
                    exit = WEXITSTATUS(status);
                    isReturn = 1;
                }
                else if((WIFSIGNALED( status ))){
                    exit = WTERMSIG(status);
                    isReturn = 0;
                }
            }
            //if is the child, it will execute the command of the
            //buffer
            else if (pid == 0){
                //int exitValue = execlp(buffer, buffer, (char*)NULL);
                int exitValue = execvp(buffer, toTerminal);
                if (exitValue == -1) {
                    writeSTDout("enseash ");
                    writeSTDout("[exit:-1]");
                    writeSTDout(" % ");
                    exit(EXIT_FAILURE);
                } else {
                    writeSTDout("enseash ");
                    char idk[10];
                    sprintf(idk, "[exit:%d]", exitValue);
                    writeSTDout(idk);
                    writeSTDout(" % ");
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
