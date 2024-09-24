#include "processes.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int fork_process_command(char* command, int bg)
{
    size_t count;
    char** tokens = tokenize(command, &count);
    return fork_process(tokens, count, bg);
}

int fork_process(char** tokens, size_t count, int bg) 
{
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execvp(tokens[0], tokens);
        char* msg = clib_str_format("Error executing command (%s):", tokens[0]);
        perror(msg);
        free(msg);
        exit(1);
    } else if (pid > 0) {
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                return SHELL_SUCCESS;
            } else if (WIFSIGNALED(status)) {
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
            } else {
                return -1;
            }
        } else {
            printf("Started background process with PID %d\n", pid);
        }
    } else {
        perror("Error forking process");
        return 1;
    }
    return 2;
}
