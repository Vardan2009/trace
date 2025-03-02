#include "lib/console.h"
#include "lib/stdio.h"
#include "lib/io.h"
#include "lib/string.h"
#include "shell.h"

void builtin_command_serialw(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 3) {
        print_info("USAGE: serialw com<1-8> <data>\n");
        return;
    }
    uint16_t port;
    if(strcmp(tokv[1], "com1") == 0) port = COM1;
    else if(strcmp(tokv[1], "com2") == 0) port = COM2;
    else if(strcmp(tokv[1], "com3") == 0) port = COM3;
    else if(strcmp(tokv[1], "com4") == 0) port = COM4;
    else if(strcmp(tokv[1], "com5") == 0) port = COM5;
    else if(strcmp(tokv[1], "com6") == 0) port = COM6;
    else if(strcmp(tokv[1], "com7") == 0) port = COM7;
    else if(strcmp(tokv[1], "com8") == 0) port = COM8;
    else return;
    serial_write_str(port, tokv[2]);
}