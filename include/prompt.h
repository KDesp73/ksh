#ifndef PROMPT_H
#define PROMPT_H

#define PROMPT_USER "user"
#define PROMPT_CWD "cwd"

#define PROMPT_RESET "0"
#define PROMPT_BOLD "bold"
#define PROMPT_UNDERLINE "under"
#define PROMPT_ITALIC "italic"

static char* prompt_styles[] = {
    PROMPT_BOLD,
    PROMPT_UNDERLINE,
    PROMPT_ITALIC,
};

#define PROMPT_BLACK "black"
#define PROMPT_RED "red"
#define PROMPT_GREEN "green"
#define PROMPT_YELLOW "yellow"
#define PROMPT_BLUE "blue"
#define PROMPT_PURPLE "purple"
#define PROMPT_CYAN "cyan"
#define PROMPT_LGREY "lgrey"
#define PROMPT_DGREY "dgrey"

static char* prompt_colors[] = {
    PROMPT_BLACK,
    PROMPT_RED,
    PROMPT_GREEN,
    PROMPT_YELLOW,
    PROMPT_BLUE,
    PROMPT_PURPLE,
    PROMPT_CYAN,
    PROMPT_LGREY,
    PROMPT_DGREY,
};

#define PROMPT_STYLE_EXAMPLE "{<style>+<color>}<text>{~}" // {bold+blue}{cwd}{~}

char* transform_brackets(const char* style);
char* transform_prompt(const char* prompt);
char* code_to_ansi(const char* code);

#endif // PROMPT_H
