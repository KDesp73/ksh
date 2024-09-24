#include "utils.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* replace_variable_with_env(const char* input)
{
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

char* trim(char* str)
{
    char* end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    return str;
}

char* extract_content(const char* str) 
{
    if (str == NULL) {
        return NULL; // Check for NULL input
    }

    size_t len = strlen(str);

    // Check for empty string
    if (len == 0) {
        return strdup(""); // Return an empty string
    }

    // Check if the first and last characters are quotes
    char quote_char = str[0];
    if ((quote_char == '"' || quote_char == '\'') && str[len - 1] == quote_char) {
        // Allocate memory for the new string without quotes
        size_t content_length = len - 2; // Exclude the quotes
        char* content = (char*)malloc(content_length + 1); // +1 for null terminator
        if (!content) {
            return NULL; // Memory allocation failed
        }
        strncpy(content, str + 1, content_length); // Copy the content
        content[content_length] = '\0'; // Null-terminate the string
        return content;
    }

    // If no quotes, return a copy of the original string
    return strdup(str); // Return a copy of the original string
}

char** insert_char_array(char** dest, size_t dest_count, char** src, size_t src_count, size_t index, size_t* new_count) 
{
    // Check if the index is valid
    if (index > dest_count) {
        return NULL; // Invalid index
    }

    // Calculate the new total count
    *new_count = dest_count + src_count;

    // Allocate memory for the new array
    char** new_array = (char**)malloc((*new_count) * sizeof(char*));
    if (!new_array) {
        return NULL; // Memory allocation failed
    }

    // Copy elements from dest up to the index
    for (size_t i = 0; i < index; i++) {
        new_array[i] = strdup(dest[i]); // Duplicate the string
        if (!new_array[i]) {
            // Free previously allocated memory in case of failure
            for (size_t j = 0; j < i; j++) {
                free(new_array[j]);
            }
            free(new_array);
            return NULL;
        }
    }

    // Insert elements from src
    for (size_t i = 0; i < src_count; i++) {
        new_array[index + i] = strdup(src[i]); // Duplicate the string
        if (!new_array[index + i]) {
            // Free previously allocated memory in case of failure
            for (size_t j = 0; j < index + i; j++) {
                free(new_array[j]);
            }
            free(new_array);
            return NULL;
        }
    }

    // Copy remaining elements from dest after the index
    for (size_t i = index; i < dest_count; i++) {
        new_array[i + src_count] = strdup(dest[i]); // Duplicate the string
        if (!new_array[i + src_count]) {
            // Free previously allocated memory in case of failure
            for (size_t j = 0; j < (*new_count); j++) {
                free(new_array[j]);
            }
            free(new_array);
            return NULL;
        }
    }

    // Free the original dest array
    for (size_t i = 0; i < dest_count; i++) {
        free(dest[i]);
    }
    free(dest);

    return new_array;
}

char** read_non_empty_lines(const char* filename, size_t* count) 
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    size_t capacity = 10;
    *count = 0;
    char** lines = (char**)malloc(capacity * sizeof(char*));
    if (!lines) {
        fclose(file);
        return NULL;
    }

    char* buffer = NULL;
    size_t len = 0;
    ssize_t line_length;

    while ((line_length = getline(&buffer, &len, file)) != -1) {
        // Strip trailing newline
        if (buffer[line_length - 1] == '\n') {
            buffer[line_length - 1] = '\0';
            line_length--;
        }

        int is_empty = 1;
        for (size_t i = 0; i < line_length; i++) {
            if (!isspace(buffer[i])) {
                is_empty = 0;
                break;
            }
        }

        if (is_empty) {
            continue;
        }

        // Reallocate memory if necessary
        if (*count >= capacity) {
            capacity *= 2;
            char** new_lines = (char**)realloc(lines, capacity * sizeof(char*));
            if (!new_lines) {
                perror("Error reallocating memory");
                free(buffer);
                fclose(file);
                return NULL;
            }
            lines = new_lines;
        }

        // Store non-empty line
        lines[*count] = strdup(buffer);
        if (!lines[*count]) {
            perror("Error allocating memory for line");
            free(buffer);
            fclose(file);
            return NULL;
        }
        (*count)++;
    }

    free(buffer);
    fclose(file);

    // Null-terminate the array
    lines[*count] = NULL;

    return lines;
}

void create_file(const char *file)
{
    if (!clib_file_exists(file)){
        clib_file_create(file);
    }
}
