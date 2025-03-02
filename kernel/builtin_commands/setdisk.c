#include "driver/diskio.h"
#include "shell.h"
#include "lib/fs.h"

void builtin_command_setdisk(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) {
        printf("USAGE: setdisk <disk number>\n");
        return;
    }
    set_drive_controller(tokv[1][0] - '0');
    strcpy(user_pwd, "/");
    init_fs();
}