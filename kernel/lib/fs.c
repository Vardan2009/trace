#include "lib/fs.h"
#include "driver/fs/iso9660.h"
#include "driver/fs/fat32.h"
#include "driver/fs/tracefs.h"
#include "lib/stdio.h"
#include "shell.h"
#include "driver/diskio.h"
#include "lib/path.h"

fs_type_t fs_type;

#define CHECK_DRIVENUM() int disk_idx = -1; \
	char *path = skip_disk_index(raw_path, &disk_idx); \
	if(disk_idx == -1) { \
		print_err("Invalid disk number `%d`", disk_idx); \
		return -1; \
	} \
	if(drive_num != disk_idx) { \
		set_drive_controller(disk_idx); \
		init_fs(); \
	}

int read_file(const char *raw_path, uint8_t *buffer, uint32_t buffer_sz) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case ISO9660: return i9660_read_file_from_path(path, buffer, buffer_sz);
	case FAT32: return fat32_read_file_from_path(path, buffer, buffer_sz);
	case TRACEFS: return tracefs_read_file_from_path(path, buffer, buffer_sz);
	default: return -1;
	}
}

int read_directory_listing(const char *raw_path, fs_entry_t dirs[256]) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case ISO9660: return i9660_read_directory(path, dirs);
	case FAT32: return fat32_list_directory(path, dirs);
	case TRACEFS: return tracefs_list_directory(path, dirs);
	default: return -1;
	}
}

int dir_exists(const char *raw_path) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case ISO9660: return i9660_dir_from_path(path) != NULL;
	case FAT32: return fat32_traverse_path(path, NULL, 1, 1) == 0;
	case TRACEFS: return tracefs_dir_exists(path);
	default: return 0;
	}
}

int create_directory(const char *raw_path) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case TRACEFS:
		print_err("TRACEFS: Empty directories not supported\n");
		return -1;
	case FAT32: return fat32_create_directory(path);
	default:
		print_err("Not available for this filesystem\n");
		return -1;
	}
}

int create_file(const char *raw_path) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case TRACEFS: return tracefs_create_file(path);
	case FAT32: return fat32_create_file(path);
	default:
		print_err("Not available for this filesystem\n");
		return -1;
	}
}

int remove_file(const char *raw_path) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case TRACEFS: return tracefs_remove_file(path);
	case FAT32: return fat32_remove_file(path);
	default:
		print_err("Not available for this filesystem\n");
		return -1;
	}
}

int write_file(const char *raw_path, const char *content) {
	CHECK_DRIVENUM();
	switch(fs_type) {
	case TRACEFS: return tracefs_write_file(path, content);
	case FAT32: return fat32_write_file_from_path(path, content, strlen(content));
	default:
		print_err("Not available for this filesystem\n");
		return -1;
	}
}

void init_fs() {
	i9660_read_pvd(&pvd);
	fat32_read_boot_sector();
	tracefs_read_header_sector();

	if(strncmp(pvd.id, "CD001", 5) == 0 && pvd.type == 1) {
		fs_type = ISO9660;
		// print_ok("Detected ISO9660 Filesystem");
	}
	else if(strncmp(fat32_boot_sector->BS_FilSysType, "FAT32   ", 8) == 0) {
		fs_type = FAT32;
		// print_ok("Detected FAT32 Filesystem");
	}
	else if(strncmp(tracefs_header_sector->fsid, "TRACEFS", 7) == 0) {
		fs_type = TRACEFS;
		// print_ok("Detected TRACEFS Filesystem");
	}
	else {
		print_err("Unknown filesystem!");
		fs_type = UNKNOWN;
		for(;;);
	}
}

fs_info_t get_fs_info() {
	switch(fs_type) {
	case ISO9660: return i9660_fs_info();
	case FAT32: return fat32_fs_info();
	case TRACEFS: return tracefs_info();
	default: return (fs_info_t){ 0 };
	}
}