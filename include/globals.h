#ifndef GLOBALS_H
#define GLOBALS_H

#include <signal.h>
extern volatile sig_atomic_t interrupted;

#define VERSION "0.1.0"
#define HISTORY_FILE clib_str_format("%s/.kshrc_history", getenv("HOME"))
#define KSHRC_FILE clib_str_format("%s/.kshrc", getenv("HOME"))

#endif // GLOBALS_H
