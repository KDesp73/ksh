#include "env.h"
#include "alias.h"
#include "history.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <unistd.h>

env_t* get_env()
{
    env_t* result = (env_t*) malloc(sizeof(env_t));

    result->cwd = clib_str_replace(getcwd(NULL, 0), getenv("HOME"), "~");
    result->user = getlogin();
    result->home = getenv("HOME");
    result->history = history_load();
    result->aliases = alias_create_table();

    return result;
}

void free_env(env_t** env)
{
    free((char*) (*env)->cwd);

    free(*env);
}
