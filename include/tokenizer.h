#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "alias.h"
#include <stdio.h>

/**
 * Splits the input string into tokens using space[s] as a delimiter
 *
 * @param input The input string
 * @param count The number of tokens found
 *
 * @returns char** The array of tokens (NULL-terminated)
 */
char** tokenize(const char* input, size_t *count);

/**
 * Recursively calls itself replacing environment variables on each token until no change occurs
 *
 * @param tokens The list of tokens
 * @param count The number of tokens
 *
 * @returns char** The modified array of tokens
 */
char** replace_env(char** tokens, size_t count);

/**
 * Recursively calls itself replacing aliases on each token until no change occurs
 *
 * @param tokens The list of tokens
 * @param count The number of tokens
 * @returns char** The modified array of tokens
 */
char** replace_aliases(alias_table_t* table, char** tokens, size_t* count);

void free_tokens(char** tokens, size_t count);
char* tokens_to_string(char** tokens, size_t count);
void print_tokens(char** tokens, size_t count);
char* tokens_to_command(char** tokens, size_t count);
int test_tokenize();

#endif // TOKENIZER_H
