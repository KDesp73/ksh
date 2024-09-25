#include "tokenizer.h"
#include "alias.h"
#include "env.h"
#include "utils.h"
#include <ctype.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <stdlib.h>
#include <string.h>

void free_tokens(char** tokens, size_t count)
{
    if (!tokens) return; // Protect against NULL pointer

    for (size_t i = 0; i < count; i++) {
        if (tokens[i]) {
            free(tokens[i]);
        }
    }
    free(tokens);
}


int search(const char* str, char c){
    for(size_t i = 0; i < strlen(str); i++){
        if (c == str[i]) return 1;
    }
    return 0;
}


char** tokenize(const char* input, size_t *count) 
{
    char* in = strdup(input);
    *count = 0;

    if (in == NULL || *in == '\0') {
        return NULL;
    }

    size_t capacity = 8;
    char** tokens = (char**)malloc(capacity + 1 * sizeof(char*));
    if (!tokens) {
        free(in);
        return NULL;
    }

    size_t i = 0;
    size_t len = strlen(in);
    while (i < len) {
        while (i < len && isspace(in[i])) i++;
        if (i >= len) break; 

        size_t start = i;
        char quote_char = 0;

        if (in[i] == '"' || in[i] == '\'') {
            quote_char = in[i];
            i++; // Move past the opening quote

            // Find the closing quote
            while (i < len && (in[i] != quote_char)) {
                i++;
            }

            // If we found the closing quote, move past it
            if (i < len && in[i] == quote_char) {
                i++;
            }
        } else {
            while (i < len && !isspace(in[i])) {
                if (in[i] == '"' || in[i] == '\'') {
                    quote_char = in[i];
                    i++; // Move past the opening quote

                    // Find the closing quote
                    while (i < len && (in[i] != quote_char)) {
                        i++;
                    }

                    // If we found the closing quote, move past it
                    if (i < len && in[i] == quote_char) {
                        i++;
                    }
                }

                i++;
            }
        }

        size_t end = i;
        if (end > start) {
            // Reallocate when at capacity
            if (*count >= capacity) {
                capacity *= 2;
                char** new_tokens = (char**)realloc(tokens, capacity+1 * sizeof(char*));
                if (!new_tokens) {
                    for (size_t j = 0; j < *count; j++) {
                        free(tokens[j]); // Free previously allocated tokens
                    }
                    free(tokens);
                    free(in);
                    return NULL;
                }
                tokens = new_tokens;
            }

            size_t token_length = end - start;

            // Allocate memory for the token and copy it
            tokens[*count] = (char*)malloc(token_length + 1);
            if (!tokens[*count]) {
                for (size_t j = 0; j < *count; j++) {
                    free(tokens[j]); // Free previously allocated tokens
                }
                free(tokens);
                free(in);
                return NULL;
            }

            strncpy(tokens[*count], in + start, token_length);
            tokens[*count][token_length] = '\0'; // Null-terminate the token
            (*count)++;
        }
    }

    tokens[*count] = NULL; // Null-terminate the array
    return tokens;
}


// Expected outputs:
// "a b c d" -> [a, b, c, d]
// "a b "c d"" -> [a,b, c d]
// "a b=c" -> [a, b=c]
// "a b="c"" -> [a, b="c"]
// "a b="c d"" -> [a, b="c d"]
int test_tokenize()
{
    char* io[][2] = {
        {"a b c d", "[a, b, c, d]"},
        {"a b 'c d'", "[a, b, 'c d']"},
        {"a b=c", "[a, b=c]"},
        {"a b='c'", "[a, b='c']"},
        {"a b='c d'", "[a, b='c d']"},
        {"alias la='ls -a'", "[alias, la='ls -a']"},
        {"alias ls='ls --color=auto'", "[alias, ls='ls --color=auto']"},
        {"ls --color=auto", "[ls, --color=auto]"},
    };

    int passed = 1;
    char* tokens_str;
    for(size_t i = 0; i < ARRAY_LEN(io); i++) {
        size_t count;
        char** tokens = tokenize(io[i][0], &count);
        tokens_str = tokens_to_string(tokens, count);
        
        if(!STREQ(io[i][1], tokens_str)){
            printf("%sfailed%s at '%s' (found: %s, expected: %s)\n", ANSI_RED, ANSI_RESET, io[i][0], tokens_str, io[i][1]);
            passed = 0;
        } else {
            printf("%spassed%s '%s'\n", ANSI_GREEN, ANSI_RESET, io[i][0]);
        }
        free(tokens_str);
        // free_tokens(tokens, count);
    }
    return passed;
}

char** replace_env(char** tokens, size_t count)
{
    int change_occured = 0;
    for(size_t i = 0; i < count; ++i){
        if(search(tokens[i], '~')) {
            tokens[i] = REPLACE_TILDA_WITH_HOME(tokens[i]);
            change_occured = 1;
        }
        if (search(tokens[i], '$')){
            tokens[i] = extract_content(tokens[i]);
            tokens[i] = replace_variable_with_env(tokens[i]);
            change_occured = 1;
        }
    }

    if(!change_occured) return tokens;

    return replace_env(tokens, count);
}

#define MAX_ALIASES 100

int alias_in_seen(char** seen_aliases, size_t seen_count, const char* alias) {
    for (size_t i = 0; i < seen_count; i++) {
        if (strcmp(seen_aliases[i], alias) == 0) {
            return 1; // Alias is already in the seen list
        }
    }
    return 0; // Alias not found in the seen list
}

int add_alias_to_seen(char** seen_aliases, size_t* seen_count, const char* alias) {
    if (*seen_count >= MAX_ALIASES) return 0;
    seen_aliases[*seen_count] = strdup(alias);
    (*seen_count)++;
    return 1; // Successfully added
}

char** replace_aliases_rec(alias_table_t* table, char** tokens, size_t* count, char** seen_aliases, size_t* seen_count);
char** replace_aliases(alias_table_t* table, char** tokens, size_t* count)
{
    // Used to stop infinite recursion when an alias references itself
    char* seen_aliases[MAX_ALIASES] = {0};
    size_t seen_count = 0;

    char** ret = replace_aliases_rec(table, tokens, count, seen_aliases, &seen_count);

    if (ret) {
        char** new_ret = realloc(ret, (*count + 1) * sizeof(char*));
        if (new_ret) {
            new_ret[*count] = NULL; // Add NULL at the end
            return new_ret;
        } else {
            return ret;
        }
    }

    return ret;
}

char** replace_aliases_rec(alias_table_t* table, char** tokens, size_t* count, char** seen_aliases, size_t* seen_count)
{
    if (table == NULL || tokens == NULL) return tokens;

    int change_occurred = 0;
    for (size_t i = 0; i < *count; ++i) {
        char* token = tokens[i];
        if (token == NULL) continue;

        if (alias_in_seen(seen_aliases, *seen_count, token)) {
            continue;
        }

        if (!add_alias_to_seen(seen_aliases, seen_count, token)) {
            // Couldn't add to seen
            continue;
        }

        char* val = alias_find(table, token);
        if (val != NULL) {
            size_t alias_tokens_count;
            char** alias_tokens = tokenize(val, &alias_tokens_count);
            if (alias_tokens == NULL) {
                continue;
            }

            if (alias_tokens_count == 1) {
                free(tokens[i]);
                // tokens[i] = strdup(alias_tokens[0]); 
            } else {
                size_t new_count;
                char** new_tokens = replace_item_with_array(tokens, *count, alias_tokens, alias_tokens_count, i, &new_count);
                if (new_tokens == NULL) {
                    free_tokens(alias_tokens, alias_tokens_count);
                    continue;
                }
                tokens = new_tokens;
                *count = new_count;
                i += alias_tokens_count - 1; // Adjust the index to skip over the newly inserted tokens
            }

            change_occurred = 1;
            free_tokens(alias_tokens, alias_tokens_count);
        }
    }

    if (!change_occurred) return tokens;

    return replace_aliases_rec(table, tokens, count, seen_aliases, seen_count);
}

char* tokens_to_string(char** tokens, size_t count)
{
    char* buffer = clib_str_buffer_init();

    clib_str_append(&buffer, "[");
    for(size_t i = 0; i < count; ++i){
        if(tokens[i] == NULL) continue;
        char* token;
        if(i == count-1){
            token = clib_str_format("%s", tokens[i]);
        } else {
            token = clib_str_format("%s, ", tokens[i]);
        }
        clib_str_append(&buffer, token);
        free(token);
    }
    clib_str_append(&buffer, "]");

    char* ret = strdup(buffer);
    free(buffer);
    return ret;
}

void print_tokens(char **tokens, size_t count)
{
    char* s = tokens_to_string(tokens, count);
    printf("%s\n", s);
    free(s);
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
        free(token_str);
    }

    return command;
}
