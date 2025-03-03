#ifndef FS_H
#define FS_H

#include <stdint.h>

typedef enum {
	UNKNOWN,
	ISO9660,
	FAT32,
	TRACEFS
} fs_type_t;

typedef struct {
	char name[64];
	int is_dir;
} fs_entry_t;

typedef struct {
	fs_type_t type;
	char *volume_name;
	int volume_name_len;
	uint64_t total_size_bytes;
} fs_info_t;

extern fs_type_t fs_type;

int read_file(const char *, uint8_t *, uint32_t);
int read_directory_listing(const char *, fs_entry_t[256]);
void init_fs();
int dir_exists(const char *);
int create_file(const char *);
int create_directory(const char *);
int remove_file(const char *);
int write_file(const char *, const char *);
fs_info_t get_fs_info();

#endif // FS_H