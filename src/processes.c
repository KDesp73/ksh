#include "processes.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void fork_process(char** tokens, int bg) 
{
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(tokens[0], tokens);
        fprintf(stderr, "Error executing command: %s\n", tokens[0]);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("Child process exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
            }
        } else {
            printf("Process %d running in the background\n", pid);
        }
    } else {
        fprintf(stderr, "Error forking process\n");
    }
}
