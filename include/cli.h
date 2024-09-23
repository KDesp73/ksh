#ifndef CLI_H
#define CLI_H

typedef enum {
    OPT_HELP = 'h',
    OPT_VERSION = 'v',
} CliOption;

void help();

#endif // CLI_H
