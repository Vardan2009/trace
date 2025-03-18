#include "shell.h"

#include "driver/pcspk.h"
#include "lib/stdio.h"

#define NCMDS 15
builtin_command_t builtin_commands[NCMDS] = {
    {"help", "help", "lists all built-in commands", &builtin_command_help},
    {"echo", "echo <text>", "prints given text to screen",
     &builtin_command_echo},
    {"clear", "clear", "clears screen", &builtin_command_clear},
    {"off", "off", "prepares system for power off", &builtin_command_off},
    {"serialw", "serialw <port> <text>", "writes data to serial port",
     &builtin_command_serialw},
    {"serialr", "serialr <port>", "listens to data in serial port",
     &builtin_command_serialr},
    {"ls", "ls", "lists current directory", &builtin_command_ls},
    {"cd", "cd <relative path>", "changes directory", &builtin_command_cd},
    {"cat", "cat <relative path>", "reads file", &builtin_command_cat},
    {"touch", "touch <relative path>", "creates new file",
     &builtin_command_touch},
    {"rm", "rm <relative path>", "removes file", &builtin_command_rm},
    {"fwrite", "fwrite <relative path> <new data>", "writes to file",
     &builtin_command_fwrite},
    {"mkdir", "mkdir <relative path>", "creates new folder",
     &builtin_command_mkdir},
    {"fsinfo", "fsinfo", "fetches info about filesystem",
     &builtin_command_fsinfo},
    {"basic", "basic [relative filepath]", "executes basic code",
     &builtin_command_basic},
};

char user_pwd[256] = "0:/";

const char *prompt() {
    static char input[512];
    set_color(COLOR_LGREEN, COLOR_BLACK);
    putc('[');
    set_color_fg(COLOR_LYELLOW);
    puts(user_pwd);
    set_color_fg(COLOR_LGREEN);
    puts("] ");
    set_color_fg(COLOR_WHITE);
    scanl(input, 512);
    set_color_fg(COLOR_WHITE);
    return input;
}

void spltoks(const char *input, char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH],
             int *token_count) {
    int token_index = 0;
    int i = 0;

    while (input[i] != '\0') {
        while (input[i] == ' ') i++;
        if (input[i] == '\0') break;

        int char_index = 0;

        if (input[i] == 'f' && input[i + 1] == '"') {
            i += 2;
            while (input[i] != '\0' && input[i] != '"') {
                if (input[i] == '\\') {
                    i++;
                    if (input[i] == 'n')
                        tokens[token_index][char_index++] = '\n';
                    else if (input[i] == 't')
                        tokens[token_index][char_index++] = '\t';
                    else if (input[i] == '\\')
                        tokens[token_index][char_index++] = '\\';
                    else if (input[i] == '"')
                        tokens[token_index][char_index++] = '"';
                    else
                        tokens[token_index][char_index++] = input[i];
                    i++;
                } else
                    tokens[token_index][char_index++] = input[i++];
            }
            if (input[i] == '"') i++;
        } else if (input[i] == '"') {
            i++;
            while (input[i] != '\0' && input[i] != '"')
                tokens[token_index][char_index++] = input[i++];
            if (input[i] == '"') i++;
        } else
            while (input[i] != '\0' && input[i] != ' ')
                tokens[token_index][char_index++] = input[i++];
        tokens[token_index][char_index] = '\0';
        token_index++;
    }
    *token_count = token_index;
}

void print_help() {
    for (int i = 0; i < NCMDS; ++i) print_help_cmd(builtin_commands[i]);
}

#define SHELL_ERR_SOUND_FREQ 500
#define SHELL_ERR_SOUND_DUR 50

void print_err(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    set_color(COLOR_WHITE, COLOR_RED);
    printf("[ ERR ] ");
    vprintf(msg, args);
    printf("\n");

    pcspk_beep_dur(SHELL_ERR_SOUND_FREQ, SHELL_ERR_SOUND_DUR);

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

    pcspk_beep_dur(SHELL_ERR_SOUND_FREQ, SHELL_ERR_SOUND_DUR);

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

    pcspk_beep_dur(SHELL_ERR_SOUND_FREQ, SHELL_ERR_SOUND_DUR);

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

    pcspk_beep_dur(SHELL_ERR_SOUND_FREQ, SHELL_ERR_SOUND_DUR);

    va_end(args);
    set_color(COLOR_WHITE, COLOR_BLACK);
}

void print_help_cmd_str(const char *command) {
    for (int i = 0; i < NCMDS; ++i) {
        if (strcmp(builtin_commands[i].name, command) == 0) {
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
            if (token_count == 2 && strcmp(tokens[1], "-?") == 0) {
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
