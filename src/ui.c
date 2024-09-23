#include "ui.h"
#include <unistd.h>
#define CLIB_MENUS
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"

#define CTRL_KEY(k) ((k) & 0x1f)

void move_right(int i) {
    printf("\033[%dC", i);
}

void move_left(int i) {
    printf("\033[%dD", i);
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
    // Check if the position is valid
    if (pos < 0 || pos >= length) {
        return; // Do nothing if the position is invalid
    }

    // Shift characters to the left to fill the gap
    for (int i = pos; i < length - 1; i++) {
        input[i] = input[i + 1];
    }

    // Null-terminate the string
    input[length - 1] = '\0'; 
}

void ui_prompt(const char* prompt, char input[]) {
    int pos = 0;
    int len = 0;

    printf("%s", prompt);
    fflush(stdout);

    char c;
    do {
        c = clib_getch();

        if (c == CLIB_KEY_BACKSPACE) {
            if (pos > 0) {
                delete_character(input, pos - 1, len);
                pos = clamp(pos - 1, 0, len - 1);
                len--;
            }
        } else if (c == CLIB_KEY_ARROW_LEFT) {
            pos = clamp(pos - 1, 0, len);
            if (pos > 0) {
                move_left(2);
            }
        } else if (c == CLIB_KEY_ARROW_RIGHT) {
            pos = clamp(pos + 1, 0, len);
            if (pos < len) {
                move_right(2);
            }
        } else if (c == CLIB_KEY_ARROW_UP) {
            // TODO: loop history
        } else if (c == CLIB_KEY_ARROW_DOWN) {
            // TODO: loop history
        } else if (c == CTRL_KEY('l')) {
            system("clear");
        } else if (c >= 32 && c < 127 && len < MAX_INPUT_LENGTH - 1) {
            insert_character(input, c, pos, len);
            pos++;
            len++;
        }

        // Clear the line and redraw the prompt and input
        printf("\r\033[K%s%s", prompt, input);
        
        fflush(stdout);

    } while (c != CLIB_KEY_ENTER);

    input[len] = '\0'; // Null-terminate the input
    printf("\n"); // Move to the next line after input is finished
}
