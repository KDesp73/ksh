#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
