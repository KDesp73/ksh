#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>


#define HISTORY_CAPACITY 10000
typedef struct {
    char** commands;
    size_t count;
} history_t;

void history_add(history_t* history, const char* command);
void history_clear(history_t* history);
char* history_to_string(const history_t* history);
history_t* history_load();
void history_setup_file();

#endif // HISTORY_H
