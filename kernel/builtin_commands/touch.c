#include "lib/fs.h"
#include "lib/path.h"
#include "shell.h"

void builtin_command_touch(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        print_info("USAGE: touch <relative filepath>");
        return;
    }
    char path[256];
    relative_to_user_pwd(tokv[1], path);
    create_file(path);
}
