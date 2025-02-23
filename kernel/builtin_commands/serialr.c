#include "lib/console.h"
#include "lib/stdio.h"
#include "lib/io.h"
#include "lib/string.h"
#include "driver/keyboard.h"
#include "shell.h"

void builtin_command_serialr(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2) return;
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
    while(kb_buf_empty())
        if(serial_received(port))
            putc(inb(port));
}