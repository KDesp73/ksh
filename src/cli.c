#include "cli.h"
#include "clib.h"

#define PTN(format, ...) \
    do { \
        fprintf(stdout, format, ##__VA_ARGS__); \
        fprintf(stdout, "\n"); \
    } while(0)

// With 2 space indent
#define PTNI(format, ...) \
    do { \
        PTN("  "format, ##__VA_ARGS__); \
    } while(0)

#define HEADER(header) \
    PTN("%s%s%s", ANSI_BOLD, header, ANSI_RESET);

void help()
{
    HEADER("USAGE");
    PTNI("ksh [-vh]");
    PTN("");

    HEADER("OPTIONS")
    PTNI("-h --help            Prints this message and exits");
    PTNI("-v --version         Prints ksh version and exits");

    PTN("");
    PTN("Made by KDesp73 (Konstantinos Despoinidis)");
}
