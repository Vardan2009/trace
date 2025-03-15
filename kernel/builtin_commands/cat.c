#include "lib/fs.h"
#include "lib/path.h"
#include "shell.h"

#define PRINT_TYPE_STRING 0
#define PRINT_TYPE_HEX 1

int isprint(char c) {
    return (c >= 32 && c <= 126);  // ASCII printable characters (space to ~)
}

void builtin_command_cat(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 2 && tokc != 3) {
        print_info("USAGE: cat <relative filepath> [-x]");
        return;
    }

    int print_type = PRINT_TYPE_STRING;
    if (tokc == 3 && strcmp(tokv[2], "-x") == 0) print_type = PRINT_TYPE_HEX;

    char path[256];
    memset(path, 0, 256);
    relative_to_user_pwd(tokv[1], path);

    static const int buflen = 60000;
    char buffer[buflen];
    int bytesread = read_file(path, buffer, buflen);
    if (bytesread == -1) return;
    buffer[bytesread] = '\0';
    switch (print_type) {
        case PRINT_TYPE_STRING: printf("%s\n", buffer); break;

        case PRINT_TYPE_HEX: {
            const int bytes_per_line = 16;
            for (int i = 0; i < bytesread; i += bytes_per_line) {
                printf("%08x  ", i);

                for (int j = 0; j < bytes_per_line; ++j) {
                    if (i + j < bytesread)
                        printf("%02x ", (unsigned char)buffer[i + j]);
                    else
                        printf("   ");
                }

                printf(" |");

                // Print ASCII representation
                for (int j = 0; j < bytes_per_line; ++j) {
                    if (i + j < bytesread) {
                        char c = buffer[i + j];
                        printf("%c", isprint(c) ? c : '.');
                    } else {
                        printf(" ");
                    }
                }
                printf("|\n");
            }
            break;
        }
    }
}
