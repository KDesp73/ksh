#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "builtins.h"
#include "env.h"
#include "history.h"
#include "processes.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "tokenizer.h"
#include "ui.h"
#include "globals.h"
#include "cli.h"

static char input[MAX_INPUT_LENGTH] = {0};
volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    interrupted = 1;
    printf("\n");
    fflush(stdout);
}

void loop(){
    env_t* env = get_env();

    system("clear");
    for(;;) {
        interrupted = 0;
        int bg = 0;
        char* prompt = clib_str_format("%s%s%s | %s%s%s > ", ANSI_BLUE, env->user, ANSI_RESET, ANSI_GREEN, env->cwd, ANSI_RESET);

        if (interrupted) {
            memset(input, 0, sizeof(input));
            free(prompt);
            continue;
        }

        ui_prompt(env, prompt, input);
        free(prompt);

        if (interrupted) {
            memset(input, 0, sizeof(input));
            continue;
        }

        size_t count;
        char** tokens = tokenize(input, &count);
        tokens = replace_env(tokens, count);

        env->last_tokens = tokens;
        env->tokens_count = count;

        // Reset input

        if(count <= 0) continue;

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

        memset(input, 0, sizeof(input));
        free_tokens(&env->last_tokens, count);
    }

    free_env(&env);
}

int main(int argc, char** argv) {
    signal(SIGINT, handle_sigint);

    CliArguments args = clib_cli_make_arguments(2, 
        clib_cli_create_argument('h', "help", "", no_argument),
        clib_cli_create_argument('v', "version", "", no_argument)
    );

    struct option* opts = clib_cli_get_options(args);
    char* fmt = clib_cli_generate_format_string(args);

    int opt;
    while ((opt = getopt_long(argc, argv, fmt, opts, ((void *)0))) != -1) {
        switch (opt) {
        case OPT_HELP:
            help();
            free(opts);
            free(fmt);
            clib_cli_clean_arguments(&args);
            exit(0);
        case OPT_VERSION:
            printf("ksh v%s\n", VERSION);
            free(opts);
            free(fmt);
            clib_cli_clean_arguments(&args);
            exit(0);
        default:
            help();
            free(opts);
            free(fmt);
            clib_cli_clean_arguments(&args);
            exit(1);
        }
    }
    free(opts);
    free(fmt);
    clib_cli_clean_arguments(&args);

    history_setup_file();

    loop();

    return 0;
}

