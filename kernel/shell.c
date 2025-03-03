#include "shell.h"

#include "lib/malloc.h"
#include "lib/stdio.h"

#include "driver/diskio.h"

#define NCMDS 13
builtin_command_t builtin_commands[NCMDS] = {
    {"help", "help", "lists all built-in commands", &builtin_command_help},
    {"echo", "echo <text>", "prints given text to screen", &builtin_command_echo},
    {"clear", "clear", "clears screen", &builtin_command_clear},
    {"off", "off", "prepares system for power off", &builtin_command_off},
    {"serialw", "serialw <port> <text>", "writes data to serial port", &builtin_command_serialw},
    {"serialr", "serialr <port>", "listens to data in serial port", &builtin_command_serialr},
    {"ls", "ls", "lists current directory", &builtin_command_ls},
    {"cd", "cd <relative path>", "changes directory", &builtin_command_cd},
    {"cat", "cat <relative path>", "reads file", &builtin_command_cat},
    {"touch", "touch <relative path>", "creates new file", &builtin_command_touch},
    {"rm", "rm <relative path>", "removes file", &builtin_command_rm},
    {"fwrite", "fwrite <relative path> <new data>", "writes to file", &builtin_command_fwrite},
    {"mkdir", "mkdir <relative path>", "creates new folder", &builtin_command_mkdir},
};

char user_pwd[256] = "0:/";

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

void print_help() {
    for(int i = 0; i < NCMDS; ++i)
            print_help_cmd(builtin_commands[i]);
}

void print_err(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    
    set_color(COLOR_WHITE, COLOR_RED);
    printf("[ ERR ] ");
    vprintf(msg, args);
    printf("\n");

    va_end(args);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void print_warn(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    set_color(COLOR_BLACK, COLOR_LYELLOW);
    printf("[ WARN ] ");
    vprintf(msg, args);
    printf("\n");

    va_end(args);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void print_info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    set_color(COLOR_WHITE, COLOR_BLUE);
    printf("[ INFO ] ");
    vprintf(msg, args);
    printf("\n");

    va_end(args);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void print_ok(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    set_color(COLOR_WHITE, COLOR_GREEN);
    printf("[ OK ] ");
    vprintf(msg, args);
    printf("\n");

    va_end(args);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void print_help_cmd_str(const char *command) {
    for(int i = 0; i < NCMDS; ++i) {
        if(strcmp(builtin_commands[i].name, command) == 0) {
            print_help_cmd(builtin_commands[i]);
            return;
        }
    }
    print_err("No such command `%s`", command);
}

void print_help_cmd(builtin_command_t cmd) {
    curx = 1;
    puts(cmd.usage);
    curx = VGA_WIDTH - strlen(cmd.description) - 1;
    puts(cmd.description);
    putc('\n');
}

void parse_command(char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int token_count) {
    for (int i = 0; i < NCMDS; ++i) {
        if (strcmp(builtin_commands[i].name, tokens[0]) == 0) {
            if(token_count == 2 && strcmp(tokens[1], "-?") == 0) {
                print_help_cmd(builtin_commands[i]);
                return;
            }
            builtin_commands[i].exec(tokens, token_count);
            return;
        }
    }
    print_err("No such command `%s`", tokens[0]);
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