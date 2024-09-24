#include "tokenizer.h"
#include "alias.h"
#include "env.h"
#include "utils.h"
#include <ctype.h>
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

char* replace_variable_with_env(const char* input) {
    const char* dollar = strchr(input, '$'); // Find the first '$'
    if (!dollar) {
        return strdup(input); // No '$' found, return a copy of the input
    }

    const char* slash = strchr(dollar, '/'); // Find the first '/' after '$'

    size_t var_length = (slash ? (slash - dollar - 1) : (strlen(dollar) - 1)); // Length of variable name excluding '$'
    

    if (var_length == 0) { // Check for zero length
        return strdup(input); // If no variable name, return the input
    }

    char* var_name = (char*)malloc(var_length + 1);
    if (var_name == NULL) {
        PANIC("Couldn't allocate memory for var_name");
    }

    strncpy(var_name, dollar + 1, var_length); // Copy the variable name
    var_name[var_length] = '\0'; // Null-terminate the variable name


    char* var_value = getenv(var_name);
    free(var_name);

    if (!var_value) {
        var_value = "";
    }

    size_t new_length = (dollar - input) + strlen(var_value) + (slash ? (strlen(slash)) : (strlen(input) - (dollar - input + 1)));

    char* result = (char*)malloc(new_length + 1); // +1 for null terminator
    if (result == NULL) {
        PANIC("Couldn't allocate memory for result");
    }

    strncpy(result, input, dollar - input);
    result[dollar - input] = '\0';

    strcat(result, var_value);

    if (slash)
        strcat(result, slash);

   return result; // Return the new string
}

int search(const char* str, char c){
    for(size_t i = 0; i < strlen(str); i++){
        if (c == str[i]) return 1;
    }
    return 0;
}

// Expected outputs:
// "a b c d" -> [a, b, c, d]
// "a b "c d"" -> [a,b, c d]
// "a b=c" -> [a, b=c]
// "a b="c"" -> [a, b="c"]
// "a b="c d"" -> [a, b="c d"]

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

    free(in);

    tokens[*count] = NULL; // Null-terminate the array
    return tokens;
}


int test_tokenize()
{
    char* io[][2] = {
        {"a b c d", "[a, b, c, d]"},
        {"a b \"c d\"", "[a, b, \"c d\"]"},
        {"a b=c", "[a, b=c]"},
        {"a b=\"c\"", "[a, b=\"c\"]"},
        {"a b=\"c d\"", "[a, b=\"c d\"]"},
        {"alias la=\"ls -a\"", "[alias, la=\"ls -a\"]"},
    };


    int passed = 1;
    for(size_t i = 0; i < ARRAY_LEN(io); i++) {
        size_t count;
        char** tokens = tokenize(io[i][0], &count);
        char* tokens_str = tokens_to_string(tokens, count);
        
        if(!STREQ(io[i][1], tokens_str)){
            printf("%sfailed%s at '%s' (found: %s, expected: %s)\n", ANSI_RED, ANSI_RESET, io[i][0], tokens_str, io[i][1]);
            passed = 0;
        } else {
            printf("%spassed%s '%s'\n", ANSI_GREEN, ANSI_RESET, io[i][0]);
        }
        free(tokens_str);
        free_tokens(&tokens, count);
    }
    return passed;
}

char** replace_env(char** tokens, size_t count)
{
    for(size_t i = 0; i < count; ++i){
        if (search(tokens[i], '$')){
            tokens[i] = extract_content(tokens[i]);
            tokens[i] = replace_variable_with_env(tokens[i]);
        }
    }
    return tokens;
}

char** replace_aliases(alias_table_t* table, char** tokens, size_t* count)
{
    for(size_t i = 0; i < *count; ++i){
        char* token = tokens[i];

        char* val = alias_find(table, token);
        if (val != NULL) {
            // DEBU("val: %s", val);
            // size_t alias_token_count;
            // char** alias_tokens = tokenize(val, &alias_token_count);
            //
            // insert_char_array(tokens, *count, alias_tokens, alias_token_count, i, count);
            tokens[i] = val;
        }
    }
    return tokens;
}

char* tokens_to_string(char** tokens, size_t count)
{
    char* buffer = clib_str_buffer_init();

    clib_str_append(&buffer, "[");
    for(size_t i = 0; i < count; ++i){
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

    return buffer;
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
    }

    return command;
}
