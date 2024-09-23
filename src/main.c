#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "builtins.h"
#include "env.h"
#include "processes.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "tokenizer.h"
#include "ui.h"
#include "globals.h"

static char input[MAX_INPUT_LENGTH] = {0};
volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    interrupted = 1;
    printf("\n");
    fflush(stdout);
}

int main() {
    signal(SIGINT, handle_sigint);

    env_t* env = get_env();

    system("clear");
    for(;;) {
        interrupted = 0;
        int bg = 0;
        char* prompt = clib_str_format("%s%s%s | %s%s%s > ", ANSI_BLUE, env->cwd, ANSI_RESET, ANSI_GREEN, env->user, ANSI_RESET);

        if (interrupted) {
            memset(input, 0, sizeof(input));
            free(prompt);
            continue;
        }

        ui_prompt(prompt, input);
        free(prompt);

        if (interrupted) {
            memset(input, 0, sizeof(input));
            continue;
        }

        size_t count;
        env->last_tokens = tokenize(input, &count);
        env->tokens_count = count;
        print_tokens(env->last_tokens, count);

        // Reset input
        memset(input, 0, sizeof(input));

        if(count <= 0) continue;

        if (STREQ("&", env->last_tokens[count-1])) {
            bg = 1;
            env->last_tokens[--env->tokens_count] = NULL;
        }

        if (is_builtin(env->last_tokens[0])) {
            exec_builtin(env);
        } else {
            fork_process(env->last_tokens, count, bg);
        }

        free_tokens(&env->last_tokens, count);
    }

    free_env(&env);

    return 0;
}

