#include "interpreter.h"
#include "builtins.h"
#include "processes.h"
#include "utils.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"
#include "tokenizer.h"
#include <stdio.h>

int interpret(env_t* env, char* input, int histincl)
{
    if(STREQ(input, "")) return 1;

    int bg = 0;
    size_t count;
    char** tokens = tokenize(input, &count);
    if (!STREQ(tokens[0], "alias")){
        tokens = replace_aliases(env->aliases, tokens, &count);
        tokens = replace_env(tokens, count);
    }

    if (!tokens){
        env->tokens_count = 0;
        env->last_tokens = NULL;
        return -1;
    }

#ifdef DEBUG
    // print_tokens(tokens, count);
#endif // DEBUG

    env->last_tokens = tokens;
    env->tokens_count = count;

    if(count <= 0) return -1;

    if (STREQ("&", env->last_tokens[count-1])) {
        bg = 1;
        env->last_tokens[--env->tokens_count] = NULL;
    }

    if (histincl && count > 0){
        history_add(env->history, input);
    }

    if (is_builtin(env->last_tokens[0])) {
        if (exec_builtin(env)) return 1;
    } else {
        char* out;
        if (fork_process(env->last_tokens, count, bg, &out)) return 2;
    }

    memset((char*) input, 0, sizeof(input));
    // free_tokens(env->last_tokens, count);

    return SHELL_SUCCESS;
}
