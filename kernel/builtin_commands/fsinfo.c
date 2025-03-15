#include "lib/fs.h"
#include "lib/stdio.h"
#include "shell.h"

void builtin_command_fsinfo(char tokv[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokc) {
    if (tokc != 1) {
        print_info("USAGE: fsinfo");
        return;
    }
    fs_info_t fs_info = get_fs_info();
    char vol_name[256];
    if (fs_info.volume_name_len > 0) {
        for (int i = 0; i < fs_info.volume_name_len; ++i)
            vol_name[i] = fs_info.volume_name[i];
        vol_name[fs_info.volume_name_len] = 0;
    } else
        strcpy(vol_name, "No Label");
    puts("Volume Label");
    curx = 40;
    printf("%s\n", vol_name);

    puts("Type");
    curx = 40;
    switch (fs_info.type) {
        case ISO9660: puts("ISO 9660\n"); break;
        case FAT32: puts("FAT 32\n"); break;
        case TRACEFS: puts("TRACE FS\n"); break;
        default: puts("Unknown\n"); break;
    }

    puts("Total size");
    curx = 40;
    if (fs_info.total_size_bytes != -1)
        printf("%d bytes\n", fs_info.total_size_bytes);
    else
        printf("No Info\n");
}