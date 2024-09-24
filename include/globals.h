#ifndef GLOBALS_H
#define GLOBALS_H

#include <signal.h>
extern volatile sig_atomic_t interrupted;

#define VERSION "0.0.1"
#define HISTORY_FILE clib_str_format("%s/.config/ksh/history", getenv("HOME"))

#endif // GLOBALS_H
