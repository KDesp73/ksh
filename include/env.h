#ifndef ENV_H
#define ENV_H

#include "alias.h"
#include "history.h"
#include <stdio.h>
#define MAX_INPUT_LENGTH 1024

#define REPLACE_HOME(path) \
    clib_str_replace(path, getenv("HOME"), "~")


typedef struct {
    char* home;
    char* user;
    char* cwd;
    char** last_tokens;
    size_t tokens_count;
    history_t* history;
    alias_table_t* aliases;
} env_t;

env_t* get_env();
void free_env(env_t** env);

#endif // ENV_H
