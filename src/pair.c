#include "pair.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include <stdlib.h>
#include <string.h>

key_value_pair_t key_value_parse(const char* input) {
    key_value_pair_t pair = { NULL, NULL };

    const char* equal_sign = strchr(input, '=');
    if (equal_sign == NULL) {
        return pair; // No '=' found, return empty pair
    }

    // Calculate the key length and allocate memory
    size_t key_length = equal_sign - input;
    pair.key = (char*)malloc(key_length + 1); // +1 for null terminator
    if (!pair.key) {
        return pair; // Memory allocation failure
    }

    // Copy the key
    strncpy(pair.key, input, key_length);
    pair.key[key_length] = '\0'; // Null-terminate the key

    // Start parsing the value
    const char* value_start = equal_sign + 1;

    // Handle quoted values (single or double quotes)
    if (*value_start == '"' || *value_start == '\'') {
        char quote_char = *value_start;
        value_start++;
        const char* value_end = value_start;
        while (*value_end != '\0' && *value_end != quote_char) {
            value_end++;
        }

        if (*value_end == quote_char) {
            // Allocate memory for the value and copy it
            size_t value_length = value_end - value_start;
            pair.value = (char*)malloc(value_length + 1); // +1 for null terminator
            if (!pair.value) {
                free(pair.key); // Free key if value allocation fails
                return pair;
            }
            strncpy(pair.value, value_start, value_length);
            pair.value[value_length] = '\0'; // Null-terminate the value
        }
    } else {
        // Handle non-quoted values (everything after '=')
        const char* value_end = value_start;
        while (*value_end != '\0' && *value_end != ' ') {
            value_end++;
        }

        size_t value_length = value_end - value_start;
        pair.value = (char*)malloc(value_length + 1); // +1 for null terminator
        if (!pair.value) {
            free(pair.key);
            return pair;
        }
        strncpy(pair.value, value_start, value_length);
        pair.value[value_length] = '\0';
    }

    return pair;
}



void key_value_free(key_value_pair_t* pair)
{
    if (pair->key) {
        free(pair->key);
    }
    if (pair->value) {
        free(pair->value);
    }
}
