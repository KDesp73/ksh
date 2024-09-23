#include "tokenizer.h"
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

    char** tokens = (char**)malloc((*count) * sizeof(char*));

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

    return tokens;
}

