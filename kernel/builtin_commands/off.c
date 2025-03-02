#include "lib/console.h"
#include "lib/stdio.h"
#include "shell.h"

void builtin_command_off(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        printf("USAGE: off\n");
        return;
    }
    prepare_shutdown();
    puts("It is now safe to power off\n");
    for (;;);
}