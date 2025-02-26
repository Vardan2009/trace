#include "driver/fs/iso9660.h"
#include "shell.h"

void builtin_command_ls(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    read_directory_from_path(user_pwd);
}