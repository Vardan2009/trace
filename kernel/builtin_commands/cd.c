#include "lib/fs.h"
#include "lib/path.h"
#include "shell.h"

void builtin_command_cd(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        print_info("USAGE: cd <relative filepath>");
        return;
    }
    char new_pwd[256];
    relative_to_user_pwd(tokv[1], new_pwd);
    if (dir_exists(new_pwd)) strcpy(user_pwd, new_pwd);
}