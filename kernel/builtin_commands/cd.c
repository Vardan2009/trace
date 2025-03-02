#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_cd(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        printf("USAGE: cd <relative filepath>\n");
        return;
    }
    char new_pwd[256];
    relative_to_user_pwd(tokv[1], new_pwd);
    if(dir_exists(new_pwd)) strcpy(user_pwd, new_pwd);
}