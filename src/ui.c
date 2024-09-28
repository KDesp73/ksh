#include "ui.h"
#include "utils.h"
#include <unistd.h>
#define CLIB_MENUS
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"
#include "globals.h"

#define CTRL_KEY(k) ((k) & 0x1f)


void move_right(int i) {
    if (i < 1) return;

    if (i == 1){
        printf("\033[C");
    } else {
        printf("\033[%dC", i);
    }
}

void move_left(int i) {
    if (i < 1) return;

    if (i == 1){
        printf("\033[D");
    } else {
        printf("\033[%dD", i);
    }
}

int clamp(int num, int min, int max){
    if(num < min) return min;
    if(num > max) return max;

    return num;
}

void insert_character(char input[], char c, int pos, int length) {
    if (pos < 0 || pos > length || length >= MAX_INPUT_LENGTH - 1) {
        return;
    }

    for (int i = length; i > pos; i--) {
        input[i] = input[i - 1];
    }

    input[pos] = c;

    input[length + 1] = '\0'; 
}

void delete_character(char input[], int pos, int length) {
    if (pos < 0 || pos >= length) {
        return;
    }

    for (int i = pos; i < length - 1; i++) {
        input[i] = input[i + 1];
    }

    input[length - 1] = '\0'; 
}

void ui_prompt(env_t* env, const char* prompt, char input[]) 
{
    clib_disable_input_buffering();
    printf("\033[?25h");

    int pos = 0;
    int len = 0;
    int history_index = env->history->count;

    printf("%s", prompt);
    fflush(stdout);

    char c;
    do {
        if (interrupted) {
            printf("\r\033[K");
            input[0] = '\0'; // Clear the input
            clib_enable_input_buffering();
            interrupted = 0;
            return;
        }
        c = clib_getch();

        if (c == CLIB_KEY_BACKSPACE) {
            if (pos > 0) {
                delete_character(input, pos - 1, len);
                pos = clamp(pos - 1, 0, len - 1);
                len--;
            }
        } else if (c == CLIB_KEY_ARROW_LEFT) {
            if (pos > 0) { 
                pos--;
                move_left(1);
            }
        } else if (c == CLIB_KEY_ARROW_RIGHT) {
            if (pos < len) { 
                pos++;
                move_right(1);
            }
        } else if (c == CLIB_KEY_ARROW_UP) {
            if (history_index > 0) {
                history_index--;

                while (history_index > 0 && STREQ(env->history->commands[history_index], env->history->commands[history_index - 1])) {
                    history_index--;
                }
            }

            if (history_index >= 0 && history_index < env->history->count) {
                strcpy(input, env->history->commands[history_index]); 
                pos = len = strlen(input);
                move_right(pos);
            }

        } else if (c == CLIB_KEY_ARROW_DOWN) {
            if (history_index < env->history->count) {
                history_index++;

                while (history_index < env->history->count - 1 && STREQ(env->history->commands[history_index], env->history->commands[history_index + 1])) {
                    history_index++;
                }
            }

    // Handle the case where we're back at the current input (no history)
    if (history_index == env->history->count) {
        strcpy(input, ""); // Clear input if we're past the last history entry
        pos = len = 0;
    } else if (history_index < env->history->count) {
        strcpy(input, env->history->commands[history_index]); 
        pos = len = strlen(input);
        move_right(pos);
    }
        } else if (c == CTRL_KEY('l')) {
            system("clear");
            printf("%s%s", prompt, input);
            move_right(pos);
        } else if (c >= 32 && c < 127 && len < MAX_INPUT_LENGTH - 1) {
            insert_character(input, c, pos, len);
            pos++;
            len++;
        }

        // Clear the line and redraw the prompt and input
        printf("\r\033[K%s%s", prompt, input);
        printf("\033[%dG", (int)(visible_length(prompt) + pos + 1));        
        printf("\033[?25h");

        fflush(stdout);
    } while (c != CLIB_KEY_ENTER);

    input[len] = '\0'; // Null-terminate the input
    printf("\n"); // Move to the next line after input is finished

    clib_enable_input_buffering();
}
