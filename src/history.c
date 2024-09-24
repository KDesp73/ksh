#include "history.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "globals.h"

void history_setup_file()
{
    char* dir = clib_str_format("%s/.config/ksh", getenv("HOME"));
    if (!clib_file_directory_exists(dir)){
        clib_file_create_directory(dir);
    }
    free(dir);

    char*file = HISTORY_FILE;
    if (!clib_file_exists(file)){
        clib_file_create(file);
    }
    free(file);
}

char* history_to_string(const history_t* history)
{
    char* res = clib_str_buffer_init();
    for(size_t i = 0; i < history->count; i++){
        clib_str_append_ln(&res, history->commands[i]);
    }
    return res;
}

void history_add(history_t* history, const char* command) 
{
    if (history->count >= HISTORY_CAPACITY) {
        free(history->commands[0]);
        for (size_t i = 1; i < history->count; i++) {
            history->commands[i - 1] = history->commands[i];
        }
        history->count--;
    }

    history->commands[history->count] = strdup(command);
    history->count++;

    char* hstr = history_to_string(history);
    clib_file_write(HISTORY_FILE, hstr, "w");
    free(hstr);
}

void history_clear(history_t* history)
{
    history->count = 0;
    history->commands = NULL;
    clib_file_delete(HISTORY_FILE);
}

history_t* history_load()
{
    char* in = clib_file_read(HISTORY_FILE, "r");

    char* input_copy = strdup(in);
    if (!input_copy) {
        return NULL;
    }

    history_t* h = (history_t*) malloc(sizeof(history_t));


    char* token = strtok(input_copy, "\n");
    h->commands = NULL;
    h->count = 0;
    
    size_t capacity = HISTORY_CAPACITY;
    h->commands = (char**)malloc(capacity * sizeof(char*));
    if (!h->commands) {
        free(input_copy);
        return NULL;
    }

    while (token != NULL) {
        h->commands[h->count] = strdup(token);
        if (!h->commands[h->count]) {
            for (size_t j = 0; j < h->count; j++) {
                free(h->commands[j]);
            }
            free(h->commands);
            free(input_copy);
            return NULL;
        }

        (h->count)++;
        token = strtok(NULL, "\n");
    }

    free(input_copy);

    h->commands[h->count] = NULL;

    return h;
}
