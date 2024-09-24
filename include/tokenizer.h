#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "alias.h"
#include <stdio.h>
char** tokenize(const char* in, size_t* count);

char** replace_env(char** tokens, size_t count);
char** replace_aliases(alias_table_t* table, char** tokens, size_t count);

void free_tokens(char*** tokens, size_t count);
char* tokens_to_string(char** tokens, size_t count);
void print_tokens(char** tokens, size_t count);
char* tokens_to_command(char** tokens, size_t count);
int test_tokenize();

#endif // TOKENIZER_H
