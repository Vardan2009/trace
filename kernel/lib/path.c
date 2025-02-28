#include "lib/path.h"
#include "lib/string.h"
#include "lib/malloc.h"
#include "lib/stdio.h"

void get_directory(const char *path, char *dir) {
    const char *last_slash = strrchr(path, '/');
    
    if (last_slash != NULL) {
        if (last_slash == path) {
            dir[0] = '/';
            dir[1] = '\0';
        } else {
            size_t len = last_slash - path;
            strncpy(dir, path, len);
            dir[len] = '\0';
        }
    } else
        dir[0] = '\0';
}

size_t split_path(const char *path, char **components, size_t max_components) {
    size_t count = 0;
    size_t len = strlen(path);
    size_t start = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (path[i] == '/') {
            if (i > start && count < max_components) {
                size_t comp_len = i - start;
                components[count] = (char *)path + start;
                count++;
            }
            start = i + 1; // Move start to the character after '/'
        }
    }

    if (start < len && count < max_components)
        components[count++] = (char *)path + start;
    
    return count;
}

void join_path(const char *base, const char *relative, char *result) {
    size_t i = 0;
    while (base[i] != '\0') {
        result[i] = base[i];
        i++;
    }

    if (result[i - 1] != '/') {
        result[i] = '/';
        i++;
    }

    size_t j = 0;
    while (relative[j] != '\0') {
        result[i++] = relative[j++];
    }

    result[i] = '\0';
}

void normalize_path(char *path) {
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

    // Copy result back to input
    strcpy(path, normalized);
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
