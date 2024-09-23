#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
char** tokenize(const char* in, size_t* count);
char** replace_env(char** tokens, size_t count);
void free_tokens(char*** tokens, size_t count);
void print_tokens(char** tokens, size_t count);
char* tokens_to_command(char** tokens, size_t count);

#endif // TOKENIZER_H
