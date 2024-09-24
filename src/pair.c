#include "pair.h"
#include <stdlib.h>
#include <string.h>

key_value_pair_t key_value_parse(const char* input)
{
    key_value_pair_t pair = { NULL, NULL };

    const char* equal_sign = strchr(input, '=');
    if (equal_sign == NULL) {
        return pair;
    }

    size_t key_length = equal_sign - input;
    size_t value_length = strlen(equal_sign + 1);

    pair.key = (char*)malloc(key_length + 1); // +1 for null terminator
    pair.value = (char*)malloc(value_length + 1);

    if (pair.key != NULL && pair.value != NULL) {
        strncpy(pair.key, input, key_length);
        pair.key[key_length] = '\0';

        strcpy(pair.value, equal_sign + 1);
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
