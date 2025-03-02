#include "driver/fs/tracefs.h"
#include "driver/diskio.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "lib/path.h"
#include "shell.h"


tracefs_header_sector_t *tracefs_header_sector;
uint8_t tracefs_boot_sector_buffer[512];

void tracefs_read_header_sector(void) {
    ata_read_sector(0, tracefs_boot_sector_buffer);
    tracefs_header_sector = (tracefs_header_sector_t *)tracefs_boot_sector_buffer;
}

int tracefs_read_file_from_path(const char *path, uint8_t *buffer, uint32_t buffer_sz) {
	char dirpath[32];
	char filename[16];
	get_directory(path, dirpath);
	get_filename(path, filename);

	char lbuffer[512];
	tracefs_file_entry_t *f_entry;
	for(int i = 1;; ++i) {
		ata_read_sector(i, lbuffer);
		f_entry = (tracefs_file_entry_t *)lbuffer;
		if(f_entry->directory[0] == 0) {
			print_err("TRACEFS: File not found");
			return -1;
		}
		if(strcmp(dirpath, f_entry->directory) == 0 && strcmp(filename, f_entry->filename) == 0) {
			strncpy(buffer, f_entry->content, buffer_sz);
			return strlen(buffer);
		}
	}
	print_err("TRACEFS: File not found");
	return -1;
}

int tracefs_list_directory(const char *dirpath, fs_entry_t dirs[256]) {
    int dirs_counter = 0;
    char lbuffer[512];
    tracefs_file_entry_t *f_entry;
    int dirpath_len = strlen(dirpath);

    char normalized_dirpath[33];
    strcpy(normalized_dirpath, dirpath);
    if (normalized_dirpath[dirpath_len - 1] != '/') {
        normalized_dirpath[dirpath_len] = '/';
        normalized_dirpath[dirpath_len + 1] = '\0';
        dirpath_len++;
    }

    for (int i = 1;; ++i) {
        ata_read_sector(i, lbuffer);
        f_entry = (tracefs_file_entry_t *)lbuffer;
        if (f_entry->directory[0] == 'd') continue;
        if (f_entry->directory[0] != '/') break;

        if (strcmp(dirpath, f_entry->directory) == 0) {
            strcpy(dirs[dirs_counter].name, f_entry->filename);
            dirs[dirs_counter].is_dir = 0;
            dirs_counter++;
        } 
        else if (strncmp(f_entry->directory, normalized_dirpath, dirpath_len) == 0) {
            char subdir[32];
            int j = 0;
            int offset = dirpath_len;
            while (f_entry->directory[offset] != '\0' &&
                   f_entry->directory[offset] != '/' &&
                   j < sizeof(subdir) - 1) {
                subdir[j++] = f_entry->directory[offset++];
            }
            subdir[j] = '\0';

            if (j > 0) {
                int exists = 0;
                for (int k = 0; k < dirs_counter; k++) {
                    if (strcmp(dirs[k].name, subdir) == 0) {
                        exists = 1;
                        break;
                    }
                }
                if (!exists) {
                    strcpy(dirs[dirs_counter].name, subdir);
                    dirs[dirs_counter].is_dir = 1;
                    dirs_counter++;
                }
            }
        }
    }
    return dirs_counter;
}

int tracefs_dir_exists(const char *dirpath) {
	char lbuffer[512];
	tracefs_file_entry_t *f_entry;
	for(int i = 1;; ++i) {
		ata_read_sector(i, lbuffer);
		f_entry = (tracefs_file_entry_t *)lbuffer;
		if(f_entry->directory[0] == 0) {
			print_err("TRACEFS: Directory not found");
			return 0;
		}
		if(strncmp(dirpath, f_entry->directory, strlen(dirpath)) == 0) return 1;
	}
	return 0;
}

void tracefs_create_file(const char *path) {
	char dirpath[32];
	char filename[16];
	get_directory(path, dirpath);
	get_filename(path, filename);

	char lbuffer[512];
	tracefs_file_entry_t *f_entry;
	for(int i = 1;; ++i) {
		ata_read_sector(i, lbuffer);
		f_entry = (tracefs_file_entry_t *)lbuffer;
		if(f_entry->directory[0] != '/' || f_entry->directory[0] == 'd') {
			//write
			strcpy(f_entry->directory, dirpath);
			strcpy(f_entry->filename, filename);
			strcpy(f_entry->content, "test content");
			ata_write_sector(i, (char*)f_entry);
			return;
		}
	}
}

int tracefs_remove_file(const char *path) {
    char dirpath[32];
    char filename[16];
    get_directory(path, dirpath);
    get_filename(path, filename);

    char lbuffer[512];
    tracefs_file_entry_t *f_entry;
    for (int i = 1;; ++i) {
        ata_read_sector(i, lbuffer);
        f_entry = (tracefs_file_entry_t *)lbuffer;
        if (f_entry->directory[0] == 0) {
            print_err("TRACEFS: File not found\n");
            return -1;
        }
        if (strcmp(dirpath, f_entry->directory) == 0 &&
            strcmp(filename, f_entry->filename) == 0) {
            memset(lbuffer, 0, 512);
        	lbuffer[0] = 'd';
            ata_write_sector(i, lbuffer);
            return 0;
        }
    }
    print_err("TRACEFS: File not found\n");
    return -1;
}

int tracefs_write_file(const char *path, const char *content) {
    char dirpath[32];
    char filename[16];
    get_directory(path, dirpath);
    get_filename(path, filename);

    char lbuffer[512];
    tracefs_file_entry_t *f_entry;
    for (int i = 1;; ++i) {
        ata_read_sector(i, lbuffer);
        f_entry = (tracefs_file_entry_t *)lbuffer;
        if (f_entry->directory[0] == 0) {
            print_err("TRACEFS: File not found\n");
            return -1;
        }
        if (strcmp(dirpath, f_entry->directory) == 0 &&
            strcmp(filename, f_entry->filename) == 0) {
            strcpy(f_entry->content, content);
            ata_write_sector(i, (char*)f_entry);
            return 0;
        }
    }
    print_err("TRACEFS: File not found\n");
    return -1;
}