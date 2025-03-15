#ifndef PATH_H
#define PATH_H

#include <stddef.h>

int has_disk_index(const char *);
char *skip_disk_index(const char *, int *);

void get_directory(const char *, char *);
void split_filename(const char *, char *, char *);
void convert_to_fat32(const char *, char *);
// void join_path(const char *, const char *, char *);
void normalize_path(char *);
void get_filename(const char *, char *);
void relative_to_user_pwd(const char *, char *);

#endif  // PATH_H
