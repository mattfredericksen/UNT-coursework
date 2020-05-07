#include "piping.h"

void pipe_parse(char* command[]) {
    int idx = 0;
    int start = 0;
    int fd[2];

    while (command[idx] != NULL) {
        if (command[idx][0] == '|') {
            if (command[idx + 1] != NULL) {
                if (pipe(fd) == -1) {
                    perror("Pipe");
                    exit(1);
                }

                pid_t pid = fork();

                if (pid == 0) {                   
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);

                    char* newCommand[idx];

                    int count = 0;
                    for (int i = start; i < idx; i++) {
                        if (command[i][0] == '|') break;

                        if (command[i] != NULL) {
                            newCommand[count] = command[i];
                        }

                        count++;
                    }

                    newCommand[idx] = NULL;

                    execvp(command[start], newCommand);
                    exit(3);
                } else if (pid > 0) {
                    // Parent
                    waitpid(pid, (int*) 0, 0);
                    dup2(fd[0], 0);
                    close(fd[0]);
                    close(fd[1]);

                    start = idx + 1;

                    int count = 0;
                    while (command[start + count] != NULL) {
                        count++;
                    }

                    char* newCommand[count];
                    count = 0;

                    while (command[start + count] != NULL) {
                        newCommand[count] = command[start + count];
                        count++;
                    }

                    newCommand[count] = NULL;

                    pid_t parent = fork();

                    if (parent == 0) {
                        execvp(command[start], newCommand);
                        printf("%s: command not found\n", command[start]);
                        exit(EXIT_FAILURE);
                    } else {
                        waitpid(parent, (int*) 0, 0);
                    }

                    
                } else {
                    perror("Fork");
                }
            }
        }

        idx++;
    }
}