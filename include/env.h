#ifndef ENV_H
#define ENV_H

#include <stdio.h>
#define MAX_INPUT_LENGTH 1000

typedef struct {
    char* home;
    char* user;
    char* cwd;
    char** last_tokens;
    size_t tokens_count;
} env_t;

env_t* get_env();
void free_env(env_t** env);

#endif // ENV_H
