#include "lib/stdio.h"
#include "shell.h"

void builtin_command_echo(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        print_info("USAGE: echo <string>");
        return;
    }
    printf(tokv[1]);
}