#include "ui.h"
#include "history.h"
#include "utils.h"
#include <unistd.h>
#define CLIB_MENUS
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "env.h"
#include "globals.h"

#define CTRL_KEY(k) ((k) & 0x1f)

void move_right(int i);
void move_left(int i);
int clamp(int num, int min, int max);
void insert_character(char input[], char c, int pos, int length);
void delete_character(char input[], int pos, int length);
char** match_history(char* match, char** history, size_t *count);
void loop_through_history(char** history, size_t count, int* history_index, char c);


void ui_prompt(env_t* env, const char* prompt, char input[]) 
{
    clib_disable_input_buffering();
    printf("\033[?25h");

    int pos = 0;
    int len = 0;
    int history_index = env->history->count;
    char user_input[MAX_INPUT_LENGTH] = {0};
    strcpy(user_input, "");

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
            strcpy(user_input, input);
            history_index = env->history->count;
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
        } else if (c == CLIB_KEY_ARROW_UP || c == CLIB_KEY_ARROW_DOWN) {
            int direction = (c == CLIB_KEY_ARROW_UP) ? -1 : 1;
            static char last_accepted_cmd[MAX_INPUT_LENGTH] = "";  // To track the last accepted history command

            // Save user input if the up arrow is pressed and the input is not empty
            if (c == CLIB_KEY_ARROW_UP && !is_empty(input) && history_index == env->history->count) {
                strcpy(user_input, input);
            }

            if ((direction == -1 && history_index > 0) || (direction == 1 && history_index < env->history->count)) {
                history_index += direction;

                // Loop to skip irrelevant commands, duplicates, and commands that match the last accepted command
                while (history_index >= 0 && history_index < env->history->count) {
                    const char* current_cmd = env->history->commands[history_index];

                    // Skip if the current command is identical to user_input or identical to the last accepted command
                    if (STREQ(current_cmd, user_input) || STREQ(current_cmd, last_accepted_cmd)) {
                        REMOTE_LOG("/dev/pts/0", "Skipped identical [%zu](%s)", history_index, current_cmd);
                    }
                    // Only show commands that start with user_input if it's not empty
                    else if (is_empty(user_input) || starts_with(current_cmd, user_input)) {
                        // Skip duplicates: only move forward if the next command is different
                        if (history_index + direction >= 0 && history_index + direction < env->history->count) {
                            const char* next_cmd = env->history->commands[history_index + direction];
                            if (!STREQ(current_cmd, next_cmd)) {
                                // Accept and track the current command
                                strcpy(last_accepted_cmd, current_cmd);
                                REMOTE_LOG("/dev/pts/0", "Accepted [%zu](%s)", history_index, current_cmd);
                                break;
                            }
                        } else {
                            // Accept and track the current command if no duplicate to check
                            strcpy(last_accepted_cmd, current_cmd);
                            REMOTE_LOG("/dev/pts/0", "Accepted (no duplicate to check) [%zu](%s)", history_index, current_cmd);
                            break;
                        }
                    }

                    REMOTE_LOG("/dev/pts/0", "Skipped [%zu](%s)", history_index, current_cmd);
                    history_index += direction;

                    // Prevent underflow in case history_index goes negative
                    if (history_index < 0) {
                        history_index = 0;
                        break;
                    }
                }

                if (history_index == env->history->count) {
                    // If we're past the last history entry, only reset if input matches user_input
                    if (is_empty(user_input) || starts_with(input, user_input)) {
                        strcpy(input, user_input);
                        pos = len = strlen(input);
                        move_right(pos);

                        // Clear user_input once it's restored
                        strcpy(user_input, "");
                        // Reset last accepted command
                        strcpy(last_accepted_cmd, "");
                    }
                } else if (history_index >= 0 && history_index < env->history->count) {
                    // Copy history command to input
                    strcpy(input, env->history->commands[history_index]);
                    pos = len = strlen(input);
                    move_right(pos);
                }
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


void loop_through_history(char** history, size_t count, int* history_index, char c)
{
    int direction = (c == CLIB_KEY_ARROW_UP) ? -1 : 1;

    if (*history_index == count && c == CLIB_KEY_ARROW_UP) {
        (*history_index)--;  // Move back from the end of history
    } else if (*history_index >= 0 && *history_index < count) {
        *history_index = clamp(*history_index + direction, 0, count - 1);

        while (*history_index >= 0 && *history_index < count - 1 && 
               STREQ(history[*history_index], history[*history_index + direction])) {
            *history_index = clamp(*history_index + direction, 0, count - 1);
        }
    }
}

// Might be too slow
char** match_history(char* match, char** history, size_t *count)
{
    size_t init_size = *count;
    history_t* res = history_init(); 

    for(size_t i = 0; i < init_size; i++){
        if(starts_with(history[i], match)){
            history_add(res, history[i]);
        }
    }

    *count = res->count;
    return res->commands;
}

void move_right(int i)
{
    if (i < 1) return;

    if (i == 1){
        printf("\033[C");
    } else {
        printf("\033[%dC", i);
    }
}

void move_left(int i)
{
    if (i < 1) return;

    if (i == 1){
        printf("\033[D");
    } else {
        printf("\033[%dD", i);
    }
}

int clamp(int num, int min, int max)
{
    if(num < min) return min;
    if(num > max) return max;

    return num;
}

void insert_character(char input[], char c, int pos, int length)
{
    if (pos < 0 || pos > length || length >= MAX_INPUT_LENGTH - 1) {
        return;
    }

    for (int i = length; i > pos; i--) {
        input[i] = input[i - 1];
    }

    input[pos] = c;

    input[length + 1] = '\0'; 
}

void delete_character(char input[], int pos, int length)
{
    if (pos < 0 || pos >= length) {
        return;
    }

    for (int i = pos; i < length - 1; i++) {
        input[i] = input[i + 1];
    }

    input[length - 1] = '\0'; 
}

