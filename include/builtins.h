#ifndef BUILTINS_H
#define BUILTINS_H
#include "env.h"
#include <stdio.h>

static char* builtins[] = {
    "cd",
    "echo",
    "exit",
    "clear"
};

char* cd(const char* path);
void echo(char **tokens, size_t count);

int exec_builtin(env_t* env);
int is_builtin(const char* command);

#endif // BUILTINS_H
