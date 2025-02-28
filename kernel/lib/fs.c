#include "lib/fs.h"
#include "driver/fs/iso9660.h"
#include "driver/fs/fat32.h"
#include "driver/fs/tracefs.h"
#include "lib/stdio.h"

fs_type_t fs_type;

int read_file(const char * path, uint8_t *buffer, uint32_t buffer_sz) {
	switch(fs_type) {
	case ISO9660: return i9660_read_file_from_path(path, buffer, buffer_sz);
	case FAT32: return fat32_read_file_from_path(path, buffer, buffer_sz);
	case TRACEFS: return tracefs_read_file_from_path(path, buffer, buffer_sz);
	default: return -1;
	}
}

int read_directory_listing(const char *path, char dirs[256][256]) {
	switch(fs_type) {
	case ISO9660: return i9660_read_directory(path, dirs);
	case FAT32: return fat32_list_directory(path, dirs);
	case TRACEFS: return tracefs_list_directory(path, dirs);
	default: return -1;
	}
}

int dir_exists(const char *path) {
	switch(fs_type) {
	case ISO9660: return i9660_dir_from_path(path) != NULL;
	case FAT32: return fat32_traverse_path(path, NULL, 1) == 0;
	case TRACEFS: return tracefs_dir_exists(path);
	default: return 0;
	}
}

void create_file(const char *path) {
	switch(fs_type) {
	case TRACEFS: return tracefs_create_file(path);
	default:
		printf("Not available for this filesystem\n");
		return;
	}
}

int remove_file(const char *path) {
	switch(fs_type) {
	case TRACEFS: return tracefs_remove_file(path);
	default:
		printf("Not available for this filesystem\n");
		return -1;
	}
}

int write_file(const char *path, const char *content) {
	switch(fs_type) {
	case TRACEFS: return tracefs_write_file(path, content);
	default:
		printf("Not available for this filesystem\n");
		return -1;
	}
}

void init_fs() {
	i9660_read_pvd(&pvd);
	fat32_read_boot_sector();
	tracefs_read_header_sector();

	if(strncmp(pvd.id, "CD001", 5) == 0 && pvd.type == 1) {
		fs_type = ISO9660;
		printf("Detected ISO9660 Filesystem\n");
	}
	else if(strncmp(fat32_boot_sector->BS_FilSysType, "FAT32   ", 8) == 0) {
		fs_type = FAT32;
		printf("Detected FAT32 Filesystem\n");
	}
	else if(strncmp(tracefs_header_sector->fsid, "TRACEFS", 7) == 0) {
		fs_type = TRACEFS;
		printf("Detected TRACEFS Filesystem\n");
	}
	else {
		printf("Unknown filesystem!");
		fs_type = UNKNOWN;
		for(;;);
	}
}
