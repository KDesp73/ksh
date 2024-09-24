#include "interpreter.h"
#include "builtins.h"
#include "processes.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"
#include "tokenizer.h"
#include <stdio.h>

int interpret(env_t* env, char* input)
{
    int bg = 0;
    size_t count;
    char** tokens = tokenize(input, &count);
    tokens = replace_env(tokens, count);
    tokens = replace_aliases(env->aliases, tokens, &count);
    print_tokens(tokens, count);

    env->last_tokens = tokens;
    env->tokens_count = count;

    if(count <= 0) return 1;

    if (STREQ("&", env->last_tokens[count-1])) {
        bg = 1;
        env->last_tokens[--env->tokens_count] = NULL;
    }

    if (count > 0){
        history_add(env->history, input);
    }

    if (is_builtin(env->last_tokens[0])) {
        exec_builtin(env);
    } else {
        fork_process(env->last_tokens, count, bg);
    }

    memset((char*) input, 0, sizeof(input));
    free_tokens(&env->last_tokens, count);

    return 0;
}
