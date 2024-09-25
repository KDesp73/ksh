#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdio.h>
int fork_process(char** tokens, size_t count, int bg, char** output);
int fork_process_command(char* command, int bg, char** output);

#endif // PROCESSES_H
