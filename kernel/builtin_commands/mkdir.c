#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_mkdir(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if(tokc != 2) return;
    char path[256];
    strcpy(path, user_pwd);
    if(tokv[1][0] == '/')
        strcpy(path, tokv[1]);
    else {
        if(strcmp(path, "/") != 0)
            strcat(path, "/");
        strcat(path, tokv[1]);
    }
    normalize_path(path);
    create_directory(path);
}