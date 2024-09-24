#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

char* extract_content(const char* str);
char** insert_char_array(char** dest, size_t dest_count, char** src, size_t src_count, size_t index, size_t* new_count);

#endif // UTILS_H
