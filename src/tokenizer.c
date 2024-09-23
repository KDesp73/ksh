#include "tokenizer.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <stdlib.h>
#include <string.h>

void free_tokens(char*** tokens, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        if ((*tokens)[i] != NULL)
            free((*tokens)[i]);
    }
    free(*tokens);
}

char** tokenize(const char* in, size_t* count) {
    *count = 0;
    int in_word = 0;
    size_t i;
    for (i = 0; in[i]; i++) {
        if (in[i] == ' ') {
            if (in_word) {
                (*count)++;
                in_word = 0;
            }
        } else {
            in_word = 1;
        }
    }
    if (in_word) {
        (*count)++;
    }

    char** tokens = (char**)malloc((*count+1) * sizeof(char*));

    in_word = 0;
    size_t token_idx = 0;
    i = 0;
    size_t start = 0;
    while (in[i]) {
        if (in[i] == ' ') {
            if (in_word) {
                tokens[token_idx++] = strndup(in + start, i - start);
                in_word = 0;
            }
        } else {
            if (!in_word) {
                start = i;
                in_word = 1;
            }
        }
        i++;
    }
    if (in_word) {
        tokens[token_idx++] = strndup(in + start, i - start);
    }

    tokens[*count] = NULL;

    return tokens;
}

void print_tokens(char **tokens, size_t count)
{
    printf("[");
    for(size_t i = 0; i < count; ++i){
        if(i == count-1){
            printf("%s", tokens[i]);
        } else {
            printf("%s, ", tokens[i]);
        }
    }
    printf("]\n");
}

char* tokens_to_command(char** tokens, size_t count)
{
    char* command = clib_str_buffer_init();

    for(size_t i = 0; i < count; ++i){
        char* token_str = NULL;

        if(i == count-1){
            token_str = clib_str_format("%s", tokens[i]);
        } else {
            token_str = clib_str_format("%s ", tokens[i]);
        }

        clib_str_append(&command, token_str);
    }

    return command;
}
