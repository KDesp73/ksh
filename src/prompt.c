#include "prompt.h"
#include <unistd.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROMPT_LENGTH 256

int is_color_code(const char* code);
int handle_reset_sequence(char* result, const char** start);

char* transform_prompt(const env_t* env, const char* prompt) {
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

        // Check for reset sequence {0}
        if (handle_reset_sequence(result, &end)) {
            start = end;
            continue;
        }

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
        char* transformed = transform_brackets(env, bracketed_part);
        free(bracketed_part); // Free the extracted part

        if (transformed) {
            strcat(result, transformed); // Append the transformed ANSI escape codes
            free(transformed); // Free transformed string
        }

        // Move the start pointer beyond the '}'
        start = close + 1;
    }

    // Append the reset sequence at the end of the prompt
    strcat(result, "\033[0m"); // ANSI escape code for reset

    return result; // Return the final transformed prompt
}

char* transform_brackets(const env_t* env, const char* style) {
    if (style[0] != '{' || style[strlen(style) - 1] != '}') {
        fprintf(stderr, "Invalid input format: %s. Must follow {style/color+color/style} or {style} or {color}\n", style);
        return NULL; // Invalid input
    }

    // Remove the curly braces
    char* style_content = substring(style, 1, strlen(style) - 1);

    // Split by '+'
    char* tokens[5] = {NULL, NULL, NULL, NULL, NULL}; // Allow up to 4 styles + 1 color
    int token_count = 0;

    char* token = strtok(style_content, "+");
    while (token && token_count < 5) {
        tokens[token_count++] = token;
        token = strtok(NULL, "+");
    }

    // Allocate memory for the resulting ANSI sequence
    char result[128] = {0};  // Large enough to hold all combined codes

    // Process each token and check if it's a valid style or color
    int color_applied = 0;
    for (int i = 0; i < token_count; ++i) {
        char* ansi_code = code_to_ansi(tokens[i]);

        if (ansi_code) {
            // Check if this is a color code
            if (is_color_code(tokens[i])) {
                if (color_applied) {
                    fprintf(stderr, "Error: multiple colors in one style group are not allowed.\n");
                    free(style_content);
                    return NULL;
                }
                color_applied = 1;
            }
            strcat(result, ansi_code);
        } else {
            // Handle custom codes like {user} or {cwd}
            if (STREQ(tokens[i], PROMPT_USER)) {
                strcat(result, env->user);
            } else if (STREQ(tokens[i], PROMPT_CWD)) {
                strcat(result, env->cwd);
            } else {
                fprintf(stderr, "Invalid code: %s\n", tokens[i]);
                free(style_content);
                return NULL;
            }
        }
    }

    free(style_content);

    // Return the combined ANSI sequence
    return strdup(result);
}

char* code_to_ansi(const char* code) {
    if(STREQ(code, PROMPT_RESET)) return ANSI_RESET;
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

int is_color_code(const char* code) {
    return STREQ(code, PROMPT_BLACK) || STREQ(code, PROMPT_RED) || STREQ(code, PROMPT_GREEN) ||
           STREQ(code, PROMPT_YELLOW) || STREQ(code, PROMPT_BLUE) || STREQ(code, PROMPT_PURPLE) ||
           STREQ(code, PROMPT_CYAN) || STREQ(code, PROMPT_LGREY) || STREQ(code, PROMPT_DGREY);
}

int handle_reset_sequence(char* result, const char** start) {
    if (strncmp(*start, "{0}", 3) == 0) {
        strcat(result, "\033[0m"); // ANSI escape code for reset
        *start += 3; // Move past the "{0}"
        return 1; // Reset applied
    }
    return 0; // No reset found
}

