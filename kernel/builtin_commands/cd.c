#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_cd(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if(tokc != 2) return;
    char old_pwd[256];
    strcpy(old_pwd, user_pwd);
    if(tokv[1][0] == '/') {
        strcpy(user_pwd, tokv[1]);
    } else {
        if(strcmp(user_pwd, "/") != 0)
        strcat(user_pwd, "/");
        strcat(user_pwd, tokv[1]);
    }
    normalize_path(user_pwd);
    if(!dir_exists(user_pwd)) strcpy(user_pwd, old_pwd);
}