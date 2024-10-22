#include "shell.h"

#include "lib/malloc.h"
#include "lib/stdio.h"

#define NSYSCALLS 2
builtin_command_t builtin_commands[NSYSCALLS] = {
    {"echo", "echo <text>", "prints given text to screen",
     &builtin_command_echo},
    {"clear", "clear", "clears screen", &builtin_command_clear}};

const char *prompt() {
    static char input[512];
    set_color(COLOR_LYELLOW, COLOR_BLACK);
    puts("[-> ");
    set_color_fg(COLOR_GREEN);
    scanl(input, 512);
    set_color_fg(COLOR_WHITE);
    return input;
}

void spltoks(const char *input, char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH],
             int *token_count) {
    bool in_quotes = false;
    int token_index = 0;
    int char_index = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '\"')
            in_quotes = !in_quotes;
        else if (input[i] == ' ' && !in_quotes) {
            if (char_index > 0) {
                tokens[token_index][char_index] = '\0';
                token_index++;
                char_index = 0;
            }
        } else
            tokens[token_index][char_index++] = input[i];
    }

    if (char_index > 0) {
        tokens[token_index][char_index] = '\0';
        token_index++;
    }

    *token_count = token_index;
}

void parse_command(char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int token_count) {
    for (int i = 0; i < NSYSCALLS; ++i) {
        if (strcmp(builtin_commands[i].name, tokens[0]) == 0) {
            builtin_commands[i].exec(tokens, token_count);
            return;
        }
    }
    set_color(COLOR_WHITE, COLOR_RED);
    printf("No such command `%s`\n", tokens[0]);
}

void shell() {
    while (true) {
        char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
        int token_count = 0;
        memset(tokens, 0, sizeof(tokens));

        const char *input = prompt();
        spltoks(input, tokens, &token_count);

        if (!token_count) continue;

        parse_command(tokens, token_count);

        set_color(COLOR_WHITE, COLOR_BLACK);

        if (curx != 0) putc('\n');
    }
}