#include "shell.h"

#include "lib/malloc.h"
#include "lib/stdio.h"

#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 256

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

void shell() {
    while (true) {
        char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
        int token_count;

        const char *input = prompt();
        spltoks(input, tokens, &token_count);

        printf("Executing command %s\n", tokens[0]);
    }
}