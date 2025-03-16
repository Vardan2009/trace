#include "basic/basic.h"

#include "basic/defs.h"
#include "lib/fs.h"
#include "lib/path.h"
#include "shell.h"

void builtin_command_basic(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2 && tokc != 1) {
        print_info("USAGE: basic [relative filepath]");
        return;
    }

    if (tokc == 2) {
        char path[256];
        memset(path, 0, 256);
        relative_to_user_pwd(tokv[1], path);

        static const int buflen = 60000;
        char buffer[buflen];
        int bytesread = read_file(path, buffer, buflen);
        if (bytesread == -1) return;
        buffer[bytesread] = '\0';
        run_basic(buffer);
    } else
        basic_shell();
}