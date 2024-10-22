#ifndef SHELL_H
#define SHELL_H

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 256

#include "builtin_commands/all.h"
#include "lib/stdio.h"

typedef void (*builtin_command_func_ptr_t)(
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int token_count);

typedef struct {
    const char *name;
    const char *usage;
    const char *description;
    builtin_command_func_ptr_t exec;
} builtin_command_t;

void shell();

#endif  // SHELL_H