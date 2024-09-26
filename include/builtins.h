#ifndef BUILTINS_H
#define BUILTINS_H
#include "env.h"
#include "history.h"
#include <stdio.h>

static char* builtins[] = {
    "builtins",
    "cd",
    "echo",
    "exit",
    "clear",
    "history",
    "export",
    "alias", "unalias",
    "source",
#ifdef DEBUG
    "test_tokenizer",
#endif // DEBUG
};

char* cd(const char* path);
void echo(char **tokens, size_t count);
void history(const history_t* history, int limited);
void export(const char* keyvalue);
void source(env_t *env, const char* file);

int exec_builtin(env_t* env);
int is_builtin(const char* command);

#endif // BUILTINS_H
