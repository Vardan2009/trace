#include "lib/path.h"
#include "lib/string.h"

void get_directory(const char *path, char *dir) {
    size_t len = strlen(path);
    size_t i = len;
    while (i > 0 && path[i - 1] != '/') {
        i--;
    }
    size_t j = 0;
    while (i > 0 && path[i - 1] != '\0') {
        dir[j++] = path[i - 1];
        i--;
    }
    dir[j] = '\0';
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
    size_t i = 0, j = 0;
    char temp[1024]; // Temporary array for building the normalized path
    size_t len = strlen(path);

    while (i < len) {
        if (path[i] == '/') {
            if (j > 0 && temp[j - 1] != '/') {
                temp[j++] = '/';
            }
            i++;
        }

        else if (path[i] == '.' && (i == 0 || path[i - 1] == '/')) {
            i++; // Skip '.' character
        }

        else if (path[i] == '.' && path[i + 1] == '.' &&
                 (i == 0 || path[i - 1] == '/')) {
            i += 2; // Skip the ".." part
            if (j > 0) {
                // Backtrack one directory level
                while (j > 0 && temp[j - 1] != '/') {
                    j--;
                }
            }
        }
        else {

            temp[j++] = path[i++];
        }
    }

    temp[j] = '\0';

    for (size_t k = 0; k <= j; k++) {
        path[k] = temp[k];
    }
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
