#include "lib/console.h"
#include "lib/stdio.h"
#include "shell.h"

void builtin_command_off(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info("USAGE: off");
        return;
    }
    prepare_shutdown();
    print_info("It is now safe to power off");
    for (;;);
}