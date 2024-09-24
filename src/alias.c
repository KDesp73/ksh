#include "alias.h"
#include <stdlib.h>
#include <string.h>

alias_table_t* alias_create_table() 
{
    alias_table_t* table = (alias_table_t*)malloc(sizeof(alias_table_t));
    table->count = 0;
    table->capacity = ALIASES_CAPACITY;
    table->aliases = (alias_t*)malloc(ALIASES_CAPACITY * sizeof(alias_t));
    return table;
}

void alias_add(alias_table_t* table, const char* name, const char* command)
{
    // Reallocate if needed
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->aliases = (alias_t*)realloc(table->aliases, table->capacity * sizeof(alias_t));
    }

    table->aliases[table->count].name = strdup(name);
    table->aliases[table->count].command = strdup(command);
    table->count++;
}

void alias_remove(alias_table_t* table, const char* name)
{
    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->aliases[i].name, name) == 0) {
            free(table->aliases[i].name);
            free(table->aliases[i].command);

            for (size_t j = i; j < table->count - 1; j++) {
                table->aliases[j] = table->aliases[j + 1];
            }
            table->count--;
            return;
        }
    }
}

char* alias_find(alias_table_t* table, const char* name)
{
    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->aliases[i].name, name) == 0) {
            return table->aliases[i].command;
        }
    }
    return NULL;
}

void aliases_print(alias_table_t* table)
{
    for(size_t i = 0; i < table->count; ++i){
        printf("alias %s='%s'\n", table->aliases[i].name, table->aliases[i].command);
    }
}
