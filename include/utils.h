#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>


int starts_with(const char* str, const char* prefix);
void print_visible(const char *str);
int is_empty(const char* str);
int is_in(const char** list, size_t size, const char* string);
char* substring(const char* str, int start, int end);
int search(const char* str, char c);
int visible_length(const char* str);
char* extract_content(const char* str);
char** replace_item_with_array(char** dest, size_t dest_count, char** src, size_t src_count, size_t index, size_t* new_count);
char** read_non_empty_lines(const char* filename, size_t* count);
void create_file(const char* file);
char* trim(char* str);
// Recursive
char* replace_variable_with_env(const char* input);

#endif // UTILS_H
