#ifndef PATH_H
#define PATH_H

#include <stddef.h> 

void get_directory(const char *, char *dir);
size_t split_path(const char *, char **, size_t);
void join_path(const char *, const char *, char *);
void normalize_path(char *);
void get_filename(const char *, char *);

#endif // PATH_H
