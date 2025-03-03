#include "lib/path.h"
#include "lib/string.h"
#include "lib/malloc.h"
#include "lib/stdio.h"
#include "lib/stdlib.h"
#include "shell.h"

int has_disk_index(const char *path) {
    return isdigit(path[0]) && path[1] == ':';
}

char *skip_disk_index(const char *path, int *disk_index) {
    if (has_disk_index(path)) {
        if (disk_index) *disk_index = path[0] - '0';
        return (char *)(path + 2);
    }
    if (disk_index) *disk_index = -1;
    return (char *)path;
}

void get_directory(const char *path, char *dir) {
    int disk_index = -1;
    const char *skipped_path = skip_disk_index(path, &disk_index);
    const char *last_slash = strrchr(skipped_path, '/');

    if (last_slash != NULL) {
        size_t len = last_slash - skipped_path;
        if (len == 0) len = 1;
        if(disk_index != -1) {
            dir[0] = disk_index + '0';
            dir[1] = 0;
            strncat(dir, ":", 1);
        }
        if(disk_index != -1) strncat(dir, skipped_path, len);
        else strncpy(dir, skipped_path, len);
    } else dir[0] = '\0';
}

void split_filename(const char *filename, char *name, char *extension) {
    const char *dot = strrchr(filename, '.');
    
    if (dot && dot != filename) {
        size_t name_len = dot - filename;
        strncpy(name, filename, name_len);
        name[name_len] = '\0';
        strcpy(extension, dot + 1);
    } else {
        strcpy(name, filename);
        extension[0] = '\0';
    }
}

void convert_to_fat32(const char *filename, char *fat_filename) {
    char name[9] = {0};
    char extension[4] = {0};
    const char *dot = strrchr(filename, '.');
    if (dot) {
        size_t name_len = dot - filename;
        if (name_len > 8) name_len = 8;
        strncpy(name, filename, name_len);
        size_t ext_len = strlen(dot + 1);
        if (ext_len > 3) ext_len = 3;
        strncpy(extension, dot + 1, ext_len);
    } else
        strncpy(name, filename, 8); // Ensure it fits in 8 characters
    for (int i = 0; i < 8 && name[i] != '\0'; i++)
        name[i] = toupper(name[i]);
    for (int i = 0; i < 3 && extension[i] != '\0'; i++)
        extension[i] = toupper(extension[i]);
    for (int i = 0; i < 8; i++)
        fat_filename[i] = (i < strlen(name)) ? name[i] : ' ';
    for (int i = 8; i < 11; i++)
        fat_filename[i] = (i - 8 < strlen(extension)) ? extension[i - 8] : ' ';
}

// void join_path(const char *base, const char *relative, char *result) {
//     size_t i = 0;
//     while (base[i] != '\0') {
//         result[i] = base[i];
//         i++;
//     }

//     if (result[i - 1] != '/') {
//         result[i] = '/';
//         i++;
//     }

//     size_t j = 0;
//     while (relative[j] != '\0') {
//         result[i++] = relative[j++];
//     }

//     result[i] = '\0';
// }

void normalize_path(char *raw_path) {
    int disk_index = -1;
    char *path = skip_disk_index(raw_path, &disk_index);
    char normalized[256] = "";
    char *stack[100];
    int stackptr = 0;

    if (path[0] == '/') {
        strcpy(normalized, "/");
    }

    char *tok = strtok(path, "/");
    while (tok != NULL) {
        if (strcmp(tok, ".") == 0) {
            // Ignore "."
        } else if (strcmp(tok, "..") == 0) {
            if (stackptr > 0) {
                stackptr--;  // Go back one level
            }
        } else {
            if (stackptr < 100) {
                stack[stackptr++] = tok;
            }
        }
        tok = strtok(NULL, "/");
    }

    // Construct normalized path
    for (int i = 0; i < stackptr; ++i) {
        strcat(normalized, stack[i]);
        if (i < stackptr - 1) {
            strcat(normalized, "/");
        }
    }

    // Ensure at least "/"
    if (normalized[0] == '\0') {
        strcpy(normalized, "/");
    }

    if(disk_index != -1) {
        raw_path[0] = disk_index + '0';
        raw_path[1] = 0;
        strncat(raw_path, ":", 1);
    }
    if(disk_index != -1) strcat(raw_path, normalized);
    else strcpy(raw_path, normalized);
}

void get_filename(const char *path, char *filename) {
    size_t len = strlen(path);
    
    // Traverse the path backwards to find the last '/'
    size_t i = len;
    while (i > 0 && path[i - 1] != '/') {
        i--;
    }

    // Copy the filename starting from the last '/' to the end
    size_t j = 0;
    while (i < len) {
        filename[j++] = path[i++];
    }

    // Null-terminate the filename string
    filename[j] = '\0';
}

void relative_to_user_pwd(const char *rel_path, char *abs_path) {
    strcpy(abs_path, user_pwd);
    if(has_disk_index(rel_path))
        strcpy(abs_path, rel_path);
    else {
        if(strcmp(abs_path, "/") != 0)
            strcat(abs_path, "/");
        strcat(abs_path, rel_path);
    }
    normalize_path(abs_path);
}