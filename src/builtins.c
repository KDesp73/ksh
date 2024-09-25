#include "builtins.h"
#include "alias.h"
#include "globals.h"
#include "history.h"
#include "interpreter.h"
#include "utils.h"
#include "pair.h"
#include "processes.h"
#include "tokenizer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"

void print_builtins()
{
    for(size_t i = 0; i < ARRAY_LEN(builtins); ++i){
        printf("%s\n", builtins[i]);
    }
}

int is_builtin(const char *command)
{
    for(size_t i = 0; i < ARRAY_LEN(builtins); ++i){
        if (STREQ(builtins[i], command)) return 1;
    }
    return 0;
}

int exec_builtin(env_t* env)
{
    char* command = env->last_tokens[0];
    if(STREQ("cd", command)) {
        char* path = cd((env->tokens_count == 1) ? NULL : env->last_tokens[1]);
        if (path == NULL) {
            return -1;
        }
        env->cwd = REPLACE_HOME_WITH_TILDA(path);
    } else if(STREQ("echo", command)) {
        echo(env->last_tokens, env->tokens_count);
    } else if(STREQ("exit", command)) {
        if (env->tokens_count == 1)
            exit(0);
        else {
            int exit_code = atoi(env->last_tokens[1]);
            exit(exit_code);
        }
    } else if (STREQ("clear", command)) {
        system("clear");
    } else if(STREQ("builtins", command)) {
        print_builtins();
    } else if(STREQ("history", command)) {
        history(env->history);
    } else if(STREQ("export", command)) {
        export(env->last_tokens[1]);
    } else if(STREQ("alias", command)) {
        int pair_index = 1;
        if(env->tokens_count == 1 || STREQ(env->last_tokens[1], "-p")) {
            aliases_print(env->aliases);
            pair_index++;
        }

        if(env->tokens_count == 1) return 0;
        if(env->tokens_count == 2 && STREQ(env->last_tokens[1], "-p")) return 0;

        key_value_pair_t pair = key_value_parse(env->last_tokens[pair_index]);
        if (pair.key == NULL || pair.value == NULL) return -1;
        alias_add(env->aliases, pair.key, pair.value);
        key_value_free(&pair);
    } else if(STREQ("unalias", command)) {
        alias_remove(env->aliases, env->last_tokens[1]);
    } else if(STREQ("test_tokenizer", command)){
        test_tokenize();
    } else if(STREQ("source", command)) {
        if(env->tokens_count < 2) {
            fprintf(stderr, "No path specified\n");
            return 1;
        }
        source(env, env->last_tokens[1]);
    }

    return SHELL_SUCCESS;
}


void history(const history_t* history)
{
    for(size_t i = history->count-1; i > history->count-1-10; i--){
        printf("%s\n", history->commands[i]);
    }
}

char* cd(const char* path) 
{
    char* temp = NULL;
    if (path != NULL){
        path = clib_str_replace(path, "~", getenv("HOME"));
        path = extract_content(path);
    }

    if (path == NULL) {
        temp = getenv("HOME");
    } else if (strcmp("-", path) == 0) {
        temp = getcwd(NULL, 0);
    } else {
        temp = strdup(path);
    }

    if (!temp) {
        printf("cd: invalid path\n");
        return NULL;
    }

    if (chdir(temp) == 0) {
        if (path != NULL && strcmp("-", path) == 0) {
            return temp; // caller should free it after use
        }
        return getcwd(NULL, 0);
    }

    switch (errno) {
        case EACCES:
            printf("cd: permission denied\n");
            break;
        case EIO:
            printf("cd: IO error\n");
            break;
        case ELOOP:
            printf("cd: looping symbolic links\n");
            break;
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
            printf("cd: invalid directory\n");
            break;
    }

    if (path != NULL && strcmp("-", path) == 0) {
        free(temp);
    }

    return NULL;
}

// KEY=VALUE
void export(const char* keyvalue)
{
    key_value_pair_t pair = key_value_parse(keyvalue);
    char* value = extract_content(pair.value);
    if (setenv(pair.key, value, 1)) {
        if (errno == ENOMEM){
            free(value);
            key_value_free(&pair);
            exit(1);
        }
        puts("export: missing '='"); // errno == EINVAL
    }
    free(value);
    key_value_free(&pair);
}


// TODO:
// 1. Dont print quotes unless escaped
// 2. Working -eE
// 3. Handle "$(command)" // Maybe another place
void echo(char **tokens, size_t count)
{
    _Bool newline = true;  // By default, append a newline
    _Bool interpret_escapes = false;  // By default, don't interpret escapes

    // Parse options if they start with a '-'
    size_t i = 1;
    if (i < count && tokens[i][0] == '-') {
        for (size_t j = 1; tokens[i][j] != '\0'; ++j) {
            switch (tokens[i][j]) {
                case 'n':
                    newline = false;
                    break;
                case 'e':
                    interpret_escapes = true;
                    break;
                case 'E':
                    interpret_escapes = false;
                    break;
                default:
                    goto print_tokens;
            }
        }
        ++i;
    }

print_tokens:
    for (; i < count; ++i) {
        char *token = tokens[i];
        token = extract_content(token);
        size_t len = strlen(token);

        if ((token[0] == '"' && token[len - 1] == '"') || 
                (token[0] == '\'' && token[len - 1] == '\'')) {
            token[len - 1] = '\0';
            ++token;
        }

        if (STREQ("-e", token)) continue;
        if (STREQ("-E", token)) continue;
        if (STREQ("-n", token)) continue;

        if (interpret_escapes) {
            for (char *p = tokens[i]; *p != '\0'; ++p) {
                if (*p == '\\') {
                    switch (*(p + 1)) {
                        case 'n':
                            printf("\n");
                            ++p;
                            break;
                        case 't':
                            printf("\t");
                            ++p;
                            break;
                        case '\\':
                            printf("\\");
                            ++p;
                            break;
                        // Add more escape sequences as needed
                        default:
                            printf("\\%c", *(p + 1));
                            ++p;
                    }
                } else {
                    putchar(*p);
                }
            }
        } else {
            printf("%s", token);
        }

        if (i < count - 1) {
            printf(" ");
        }
    }

    if (newline) {
        printf("\n");
    }
}

void source(env_t *env, const char* file)
{
    size_t line_count;
    char** lines = read_non_empty_lines(file, &line_count);

    for(size_t i = 0; i < line_count; i++){
        if (lines[i] == NULL) continue;
        if (lines[i][0] == '#') continue; // Skip comments

        if (interpret(env, lines[i], 0) != SHELL_SUCCESS){
            printf("Error in line: %zu\n", i);
        }
    }
}
