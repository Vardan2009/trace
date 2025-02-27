#include "lib/fs.h"
#include "driver/fs/iso9660.h"
#include "lib/stdio.h"

fs_type_t fs_type;

int read_file(const char * path, uint8_t *buffer, uint32_t buffer_sz) {
	switch(fs_type) {
	case ISO9660: return i9660_read_file_from_path(path, buffer, buffer_sz);
	case FAT32:
	default: return -1;
	}
}

int read_directory_listing(const char *path, char dirs[256][256]) {
	switch(fs_type) {
	case ISO9660: return i9660_read_directory(path, dirs);
	case FAT32:
	default: return -1;
	}
}

int dir_exists(const char *path) {
	switch(fs_type) {
	case ISO9660: return i9660_dir_from_path(path) != NULL;
	case FAT32:
	default: return 0;
	}
}

void init_fs() {
	i9660_read_pvd(&pvd);
	if(strncmp(pvd.id, "CD001", 5) == 0 && pvd.type == 1) {
		fs_type = ISO9660;
		printf("Detected ISO9660 Filesystem\n");
	}
	else {
		printf("Unknown filesystem!");
		fs_type = UNKNOWN;
		for(;;);
	}
}
