#include "shell.h"
#include "lib/path.h"
#include "lib/fs.h"

void builtin_command_fwrite(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 3) {
        print_info("USAGE: fwrite <relative filepath> <data>");
        return;
    }
    char path[256];
    relative_to_user_pwd(tokv[1], path);
    write_file(path, tokv[2]);
}
