#ifndef ALIAS_H
#define ALIAS_H

#include <stdio.h>

typedef struct {
    char* name;
    char* command;
} alias_t;

#define ALIASES_CAPACITY 8
typedef struct {
    alias_t* aliases;
    size_t count;
    size_t capacity;
} alias_table_t;

alias_table_t* alias_create_table();
char* alias_find(alias_table_t* table, const char* name);
void alias_remove(alias_table_t* table, const char* name);
void alias_add(alias_table_t* table, const char* name, const char* command);
void aliases_print(alias_table_t* table);

#endif // ALIAS_H
