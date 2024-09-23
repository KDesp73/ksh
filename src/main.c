#include "builtins.h"
#include "env.h"
#include "processes.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "tokenizer.h"


int main(){
    env_t* env = get_env();
    char input[MAX_INPUT_LENGTH];

    for(;;){
        char* prompt = clib_str_format("%s%s%s | %s%s%s > ", ANSI_BLUE, env->cwd, ANSI_RESET, ANSI_GREEN, env->user, ANSI_RESET);
        printf("%s", prompt);
        if (fgets(input, MAX_INPUT_LENGTH, stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';
        } else {
            continue;
        }
        free(prompt);

        size_t count;
        env->last_tokens = tokenize(input, &count);
        env->tokens_count = count;

        if(count <= 0) continue;

        if (is_builtin(env->last_tokens[0])) {
            INFO("Builtin");
            exec_builtin(env);
        }


        free_tokens(&env->last_tokens, count);
    }

    free_env(&env);

    return 0;
}
