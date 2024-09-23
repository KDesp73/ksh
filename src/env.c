#include "env.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <unistd.h>

env_t* get_env()
{
    env_t* result = (env_t*) malloc(sizeof(env_t));

    result->cwd = clib_str_replace(getcwd(NULL, 0), getenv("HOME"), "~");
    result->user = getlogin();
    result->home = getenv("HOME");

    return result;
}

void free_env(env_t** env)
{
    free((char*) (*env)->cwd);

    free(*env);
}
