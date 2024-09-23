#include "builtins.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "clib.h"
#include "env.h"

int is_builtin(const char *command)
{
    for(size_t i = 0; i < ARRAY_LEN(builtins); ++i){
        if (STREQ(builtins[i], command)) return 1;
    }
    return 0;
}

int exec_builtin(env_t* env)
{
    if(STREQ("cd", env->last_tokens[0])) {
        char* path = cd(env->last_tokens[1]);
        if (path == NULL) {
            return -1;
        }
        env->cwd = path;
    } else if(STREQ("echo", env->last_tokens[0])) {
        echo(env->last_tokens, env->tokens_count);
    } else if(STREQ("exit", env->last_tokens[0])) {
        if (env->tokens_count == 1)
            exit(0);
        else {
            int exit_code = atoi(env->last_tokens[1]);
            exit(exit_code);
        }
    } else if (STREQ("clear", env->last_tokens[0])) {
        system("clear");
    }

    return 0;
}

char* cd(const char* path) 
{
    char* temp = NULL;
    if (!path)
        temp = getenv("HOME");
    else if (strcmp("~", path) == 0)
        temp = getenv("HOME");
    else if (strcmp("-", path) == 0) {
        temp = getcwd(NULL, 0);
    }

    if (!chdir(temp)) return temp;

    switch (errno) {
        case EACCES:
            printf("cd: permission denied");
            break;
        case EIO:
            printf("cd: IO error");
            break;
        case ELOOP:
            printf("cd: looping symbolic links");
            break;
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
            printf("cd: invalid directory");
    }
    return NULL;
}

// TODO: [-neE] options implementation
void echo(char **tokens, size_t count)
{
    for(size_t i = 1; i < count; ++i) {
        printf("%s ", tokens[i]); // TODO: correct printing
    }
    printf("\n");
}
