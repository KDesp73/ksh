#include "prompt.h"
#include <unistd.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROMPT_LENGTH 256

char* transform_prompt(const char* prompt) {
    if (prompt == NULL || is_empty(prompt)) return NULL;

    char* result = malloc(MAX_PROMPT_LENGTH);
    if (result == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    result[0] = '\0';

    const char* start = prompt;
    const char* end;

    while (*start) {
        // Look for the next '{'
        end = strchr(start, '{');
        if (!end) {
            // No more brackets; copy the rest of the string
            strcat(result, start);
            break;
        }
        
        // Copy everything before the '{' into result
        strncat(result, start, end - start);
        
        // Find the corresponding '}'
        const char* close = strchr(end, '}');
        if (!close) {
            fprintf(stderr, "Unmatched '{' in prompt: %s\n", prompt);
            free(result);
            return NULL; // Unmatched '{'
        }

        // Extract the bracketed part
        size_t length = close - end + 1; // Include '}'
        char* bracketed_part = substring(end, 0, length); // Allocate substring for the part
        if (bracketed_part == NULL) {
            free(result);
            return NULL; // Allocation failed
        }

        // Transform the bracketed part
        char* transformed = transform_brackets(bracketed_part);
        free(bracketed_part); // Free the extracted part

        if (transformed) {
            strcat(result, transformed); // Append the transformed ANSI escape codes
            free(transformed); // Free transformed string
        }

        // Move the start pointer beyond the '}'
        start = close + 1;
    }

    // Remove any trailing ANSI codes if they are just resets
    // This part ensures there aren't consecutive reset sequences at the end.
    char* reset_sequence = "\033[0m";
    if (strlen(result) >= strlen(reset_sequence) && strcmp(result + strlen(result) - strlen(reset_sequence), reset_sequence) == 0) {
        result[strlen(result) - strlen(reset_sequence)] = '\0'; // Remove last reset
    }

    // Append the reset sequence at the end of the prompt
    strcat(result, "\033[0m"); // ANSI escape code for reset

    return result; // Return the final transformed prompt
}

char* transform_brackets(const char* style) {
    char *ps, *pc, *single;

    if (style[0] != '{' || style[strlen(style) - 1] != '}') {
        fprintf(stderr, "Invalid input format: %s. Must follow {style/color+color/style} or {style} or {color}\n", style);
        return NULL; // Invalid input
    }

    int plus_index = search(style, '+');

    if (plus_index < 0) {
        single = substring(style, 1, strlen(style) - 1);
        char* result = code_to_ansi(single);

        if (result == NULL) { // Not ANSI
            if (STREQ(single, PROMPT_USER)) {
                char* login = getlogin();
                return login ? strdup(login) : NULL; // Check for NULL from getlogin()
            } else if (STREQ(single, PROMPT_CWD)) {
                char* cwd = getcwd(NULL, 0);
                char* replaced = clib_str_replace(cwd, getenv("HOME"), "~");
                free(cwd);
                return replaced;
            } else {
                fprintf(stderr, "Invalid code: %s\n", single);
                free(result);
                return NULL;
            }
        }
        free(single);

        return strdup(result);
    }

    ps = substring(style, 1, plus_index);
    pc = substring(style, plus_index + 1, strlen(style) - 1);

    char* ansi_ps = code_to_ansi(ps);
    free(ps);
    if (ansi_ps == NULL) {
        fprintf(stderr, "Invalid ANSI code: %s\n", ps);
        return NULL;
    }

    char* ansi_pc = code_to_ansi(pc);
    free(pc);
    if (ansi_pc == NULL) {
        fprintf(stderr, "Invalid ANSI code: %s\n", pc);
        return NULL;
    }

    char* combined = ANSI_COMBINE(ansi_ps, ansi_pc);

    return combined;
}


#define ANSI_RESET_2 ANSI_RESET ANSI_RESET
char* code_to_ansi(const char* code)
{
    if(STREQ(code, PROMPT_RESET)) return ANSI_RESET ANSI_RESET;
    else if(STREQ(code, PROMPT_BOLD)) return ANSI_BOLD;
    else if(STREQ(code, PROMPT_UNDERLINE)) return ANSI_UNDERLINE;
    else if(STREQ(code, PROMPT_ITALIC)) return ANSI_ITALIC;
    else if(STREQ(code, PROMPT_BLACK)) return ANSI_BLACK;
    else if(STREQ(code, PROMPT_RED)) return ANSI_RED;
    else if(STREQ(code, PROMPT_GREEN)) return ANSI_GREEN;
    else if(STREQ(code, PROMPT_YELLOW)) return ANSI_YELLOW;
    else if(STREQ(code, PROMPT_BLUE)) return ANSI_BLUE;
    else if(STREQ(code, PROMPT_PURPLE)) return ANSI_PURPLE;
    else if(STREQ(code, PROMPT_CYAN)) return ANSI_CYAN;
    else if(STREQ(code, PROMPT_LGREY)) return ANSI_LGREY;
    else if(STREQ(code, PROMPT_DGREY)) return ANSI_DGREY;

    return NULL;
}

