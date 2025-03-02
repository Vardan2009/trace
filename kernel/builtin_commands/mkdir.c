#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_mkdir(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        printf("USAGE: mkdir <relative filepath>\n");
        return;
    }
    char path[256];
    relative_to_user_pwd(tokv[1], path);
    create_directory(path);
}