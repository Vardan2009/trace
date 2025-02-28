#ifndef TRACEFS_H
#define TRACEFS_H

#include <stdint.h>

typedef struct {
	char fsid[7];				 // should be TRACEFS
	char metadata[512 - 7];		 // metadata
} __attribute__((packed)) tracefs_header_sector_t;

typedef struct {
	char directory[32];			 // absolute directory starting from /
	char filename[16];			 // filename
	char content[512 - 32 - 16]; // content
} __attribute__((packed)) tracefs_file_entry_t;

extern tracefs_header_sector_t *tracefs_header_sector;

void tracefs_read_header_sector();
int tracefs_read_file_from_path(const char *, uint8_t *, uint32_t);
int tracefs_list_directory(const char *, char[256][256]);
int tracefs_dir_exists(const char *);

void tracefs_create_file(const char *);
int tracefs_remove_file(const char *);

#endif // TRACEFS_H