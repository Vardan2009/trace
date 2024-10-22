#include "lib/stdio.h"
#include "shell.h"

void builtin_command_clear(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    clear_screen();
}