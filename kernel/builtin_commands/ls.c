#include "lib/fs.h"
#include "lib/string.h"
#include "shell.h"

void builtin_command_ls(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info("USAGE: ls");
        return;
    }
    fs_entry_t ls[256];
    memset(ls, 0, sizeof(ls));
    int len = read_directory_listing(user_pwd, ls);
    for (int i = 0; i < len; ++i) {
        if (ls[i].is_dir)
            set_color_fg(COLOR_LGREEN);
        else
            set_color_fg(COLOR_YELLOW);
        printf("%s%s%s", ls[i].name, ls[i].is_dir ? "/" : "",
               (curx + strlen(ls[i].name)) >= VGA_WIDTH ? "\n" : " ");
    }
    set_color_fg(COLOR_WHITE);
}