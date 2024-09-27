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
#include "interpreter.h"
#include "prompt.h"
#include "utils.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "ui.h"
#include "globals.h"
#include "cli.h"

static char input[MAX_INPUT_LENGTH] = {0};
volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) 
{
    interrupted = 1;
    printf("\n");
}

void loop(env_t* env)
{
    for(;;) {
        interrupted = 0;

        char* prompt_env = getenv("PROMPT");
        char* prompt = transform_prompt(env, prompt_env);

        if(!prompt) prompt = strdup("[{bold+blue}{user}{0}] {bold+green}{cwd}{0} > ");

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

        if(interpret(env, input, 1) != SHELL_SUCCESS) continue;
    }
}

void setup_sigint_handler() {
    struct sigaction sa;
    sa.sa_handler = &handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
}

int main(int argc, char** argv)
{
    setup_sigint_handler();

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

    env_t* env = get_env();

    system("clear");

    char* src_file = KSHRC_FILE;
    if(clib_file_exists(src_file)){
        source(env, src_file);
    }
    free(src_file);

    loop(env);

    free_env(&env);

    return 0;
}

