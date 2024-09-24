#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

char* extract_content(const char* str);
char** insert_char_array(char** dest, size_t dest_count, char** src, size_t src_count, size_t index, size_t* new_count);
char** read_non_empty_lines(const char* filename, size_t* count);
void create_file(const char* file);
char* trim(char* str);
// Recursive
char* replace_variable_with_env(const char* input);

#endif // UTILS_H
