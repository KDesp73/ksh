#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <bsd/string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"

int main(){
    const char* login = getlogin();

    INFO("login: %s", login);

    return 0;
}
