#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_fwrite(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc < 3) return;
    char path[256];
    strcpy(path, user_pwd);
    if(strcmp(user_pwd, "/") != 0) strcat(path, "/");
    strcat(path, tokv[1]);
    normalize_path(path);
    write_file(path, tokv[2]);
}
