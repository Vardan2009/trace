#include "lib/fs.h"
#include "shell.h"
#include "lib/string.h"

void builtin_command_ls(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info("USAGE: ls\n");
        return;
    }
    char ls[256][256];
    memset(ls, 0, sizeof(ls));
    for(int i = 0; i < read_directory_listing(user_pwd, ls); ++i) printf("%s ", ls[i]);
}