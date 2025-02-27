#include "lib/fs.h"
#include "shell.h"

void builtin_command_ls(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    char ls[256][256];
    for(int i = 0; i < read_directory_listing(user_pwd, ls); ++i) printf("%s ", ls[i]);
}