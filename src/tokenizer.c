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

char** tokenize(const char* in, size_t* count) {
    *count = 0;

    char* input_copy = strdup(in);
    if (!input_copy) {
        return NULL;
    }

    char* token = strtok(input_copy, " ");
    char** tokens = NULL;
    
    size_t capacity = 10;
    tokens = (char**)malloc(capacity * sizeof(char*));
    if (!tokens) {
        free(input_copy);
        return NULL;
    }

    while (token != NULL) {
        if (*count >= capacity) {
            capacity *= 2;
            char** new_tokens = (char**)realloc(tokens, capacity * sizeof(char*));
            if (!new_tokens) {
                for (size_t j = 0; j < *count; j++) {
                    free(tokens[j]);
                }
                free(tokens);
                free(input_copy);
                return NULL;
            }
            tokens = new_tokens;
        }
        
        tokens[*count] = strdup(token);
        if (!tokens[*count]) {
            for (size_t j = 0; j < *count; j++) {
                free(tokens[j]);
            }
            free(tokens);
            free(input_copy);
            return NULL;
        }

        (*count)++;
        token = strtok(NULL, " ");
    }

    free(input_copy);

    tokens[*count] = NULL;

    return tokens;
}

char** replace_env(char** tokens, size_t count)
{
    for(size_t i = 0; i < count; ++i){
        if (search(tokens[i], '$')){
            tokens[i] = replace_variable_with_env(tokens[i]);
        }
    }
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
