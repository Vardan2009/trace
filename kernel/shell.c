#include "shell.h"

#include "lib/malloc.h"
#include "lib/stdio.h"

#include "driver/diskio.h"

#define NCMDS 12
builtin_command_t builtin_commands[NCMDS] = {
    {"echo", "echo <text>", "prints given text to screen",
     &builtin_command_echo},
    {"clear", "clear", "clears screen", &builtin_command_clear},
    {"off", "off", "prepares system for power off", &builtin_command_off},
    {"serialw", "serialw <port> <text>", "writes data to serial port", &builtin_command_serialw},
    {"serialr", "serialr <port>", "listens to data in serial port", &builtin_command_serialr},
    {"ls", "ls", "lists current directory", &builtin_command_ls},
    {"cd", "cd <relative path>", "changes directory", &builtin_command_cd},
    {"cat", "cat <relative path>", "read file", &builtin_command_cat},
    {"setdisk", "setdisk <disk num>", "set current disk", &builtin_command_setdisk},
    {"touch", "touch <relative path>", "create new file", &builtin_command_touch},
    {"rm", "rm <relative path>", "remove file", &builtin_command_rm},
    {"fwrite", "fwrite <relative path> <new data>", "write to file", &builtin_command_fwrite},
};

char user_pwd[256] = "/";

const char *prompt() {
    static char input[512];
    set_color(COLOR_LYELLOW, COLOR_BLACK);
    printf("[DISK %d] %s -> ", drive_num, user_pwd);
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
    for (int i = 0; i < NCMDS; ++i) {
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