#include "lib/stdio.h"
#include "shell.h"

void builtin_command_help(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info(
            "USAGE: help\nTo get info about a specific built-in command, run "
            "the command with -? flag");
        return;
    }
    print_help();
}