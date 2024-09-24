#include "processes.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void fork_process_command(char* command, int bg)
{
    size_t count;
    char** tokens = tokenize(command, &count);
    fork_process(tokens, count, bg);
}

void fork_process(char** tokens, size_t count, int bg) 
{
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execvp(tokens[0], tokens);
        perror("Error executing command");
        exit(1);
    } else if (pid > 0) {
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                return;
            } else if (WIFSIGNALED(status)) {
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
            }
        } else {
            printf("Started background process with PID %d\n", pid);
        }
    } else {
        perror("Error forking process");
    }
}
