#ifndef BUILTINS_H
#define BUILTINS_H
#include "env.h"
#include "history.h"
#include <stdio.h>

static char* builtins[] = {
    "builtins",
    "cd",
    "echo",
    "exit", "quit",
    "clear",
    "history",
};

char* cd(const char* path);
void echo(char **tokens, size_t count);
void history(const history_t* history);

int exec_builtin(env_t* env);
int is_builtin(const char* command);

#endif // BUILTINS_H
