#include "lib/stdio.h"
#include "shell.h"

void builtin_command_clear(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info("USAGE: clear\n");
        return;
    }
    clear_screen();
}