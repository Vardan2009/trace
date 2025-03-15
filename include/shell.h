#ifndef SHELL_H
#define SHELL_H

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 256

#include "builtin_commands/all.h"
#include "lib/stdio.h"

#define TRACE_LOGO \
    "  __________  ___   ____________ \n\
 /_  __/ __ \\/   | / ____/ ____/\n\
  / / / /_/ / /| |/ /   / __/   \n\
 / / / _, _/ ___ / /___/ /___   \n\
/_/ /_/ |_/_/  |_\\____/_____/   \n\
                                "

typedef void (*builtin_command_func_ptr_t)(
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int token_count);

typedef struct {
    const char *name;
    const char *usage;
    const char *description;
    builtin_command_func_ptr_t exec;
} builtin_command_t;

extern char user_pwd[256];

void shell();
void print_help();
void print_help_cmd_str(const char *);
void print_help_cmd(builtin_command_t);

void print_err(const char *, ...);
void print_warn(const char *, ...);
void print_info(const char *, ...);
void print_ok(const char *, ...);

#endif  // SHELL_H