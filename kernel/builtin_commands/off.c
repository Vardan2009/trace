#include "lib/console.h"
#include "lib/stdio.h"
#include "shell.h"

void builtin_command_off(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    prepare_shutdown();
    puts("It is now safe to power off\n");
    for (;;);
}