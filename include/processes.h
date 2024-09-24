#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdio.h>
void fork_process(char** tokens, size_t count, int bg);
void fork_process_command(char* command, int bg);

#endif // PROCESSES_H
