#include "driver/fs/iso9660.h"
#include "lib/string.h"
#include "driver/diskio.h"
#include "lib/stdio.h"
#include "lib/path.h"

i9660_pvd_t pvd;

void i9660_read_pvd(i9660_pvd_t *pvd) {
	uint8_t sector_buffer[ATA_SECTOR_SIZE];
    uint8_t *pvd_bytes = (uint8_t *)pvd;

    for (int i = 0; i < SECTORS_PER_ISO; i++) {
        ata_read_sector(PVD_LBA + i, sector_buffer);
        memcpy(pvd_bytes + (i * ATA_SECTOR_SIZE), sector_buffer, ATA_SECTOR_SIZE);
    }
}

void i9660_read_cd_sector(uint32_t cd_sector, uint8_t *buffer) {
    uint32_t base_lba = cd_sector * SECTORS_PER_ISO;
    for (int i = 0; i < SECTORS_PER_ISO; i++) {
        ata_read_sector(base_lba + i, buffer + (i * ATA_SECTOR_SIZE));
    }
}

void i9660_process_directory(uint32_t extent, uint32_t length, dir_record_callback_t callback) {
    uint32_t sectors = (length + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    uint8_t block[ISO_SECTOR_SIZE];
    for (uint32_t i = 0; i < sectors; i++) {
        i9660_read_cd_sector(extent + i, block);
        uint32_t offset = 0;
        while (offset < ISO_SECTOR_SIZE) {
            iso9660_dir_record_t *record = (iso9660_dir_record_t *)(block + offset);
            if (record->length == 0)
                break;
            callback(record);
            offset += record->length;
        }
    }
}

void i9660_get_file_identifier(iso9660_dir_record_t *record, char *dest, int dest_size) {
    int len = record->file_id_length;
    if (len > dest_size - 1)
        len = dest_size - 1;
    memcpy(dest, ((uint8_t*)record) + 33, len);
    dest[len] = '\0';
    char *semicolon = strchr(dest, ';');
    if (semicolon)
        *semicolon = '\0';
}

int i9660_compare_file_identifier(iso9660_dir_record_t *record, const char *name) {
    char filename[256];
    i9660_get_file_identifier(record, filename, sizeof(filename));
    return strcmp(filename, name);
}

iso9660_dir_record_t *i9660_find_directory_record(uint32_t extent, uint32_t length, const char *name) {
    static uint8_t found_record_buffer[256];
    int found = 0;
    uint8_t block[ISO_SECTOR_SIZE];
    uint32_t sectors = (length + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    for (uint32_t i = 0; i < sectors && !found; i++) {
        i9660_read_cd_sector(extent + i, block);
        uint32_t offset = 0;
        while (offset < ISO_SECTOR_SIZE) {
            iso9660_dir_record_t *record = (iso9660_dir_record_t *)(block + offset);
            if (record->length == 0)
                break;
            if (i9660_compare_file_identifier(record, name) == 0) {
                if (record->length < sizeof(found_record_buffer))
                    memcpy(found_record_buffer, record, record->length);
                found = 1;
                break;
            }
            offset += record->length;
        }
    }
    if (found)
        return (iso9660_dir_record_t *)found_record_buffer;
    else
        return NULL;
}


iso9660_dir_record_t *i9660_dir_from_path(const char *path) {
	iso9660_dir_record_t *current = (iso9660_dir_record_t *)pvd.root_dir;
    char token[64];
    const char *p = path;
    if (*p == '/')
        p++;  // Skip leading '/'
    while (*p) {
        int i = 0;
        while (*p && *p != '/' && i < (int)sizeof(token) - 1) {
            token[i++] = *p++;
        }
        token[i] = '\0';
        while (*p == '/')
            p++;
        uint32_t curr_extent = current->extent_lba_lsb;  // LSB used
        uint32_t curr_length = current->data_length_lsb;   // LSB used
        iso9660_dir_record_t *child = i9660_find_directory_record(curr_extent, curr_length, token);
        if (child == NULL) {
            printf("Directory '%s' not found.\n", token);
            return NULL;
        }
        if (!(child->file_flags & 0x02)) {
            printf("'%s' is not a directory.\n", token);
            return NULL;
        }
        current = child;
    }
    return current;
}

int diridx = 0;
char (*directories)[256];  // Correct pointer type

void set_dir_record(iso9660_dir_record_t *record) {
    if (record->file_id_length == 1) {
        uint8_t fid = *((uint8_t*)record + 33);
        if (fid == 0 || fid == 1)
            return;
    }
    
    char filename[256];
    i9660_get_file_identifier(record, filename, sizeof(filename));
    
    if (diridx >= 256) return;  // Prevent buffer overflow

    strcpy(directories[diridx++], filename);  // Use correct pointer type
}

int i9660_read_directory(const char *path, char dirs[256][256]) {
    diridx = 0;
    directories = dirs;  // Correctly assign the pointer

    iso9660_dir_record_t *current = i9660_dir_from_path(path);
    if (!current) return -1;

    uint32_t dir_extent = current->extent_lba_lsb;
    uint32_t dir_length = current->data_length_lsb;
    
    i9660_process_directory(dir_extent, dir_length, set_dir_record);
    return diridx;
}


int i9660_read_file_from_path(const char *path, uint8_t *buffer, uint32_t buffer_size) {
	char dirpath[256];
	char filename[256];
	get_directory(path, dirpath);
	get_filename(path, filename);

    iso9660_dir_record_t *current = i9660_dir_from_path(dirpath);
    uint32_t curr_extent = current->extent_lba_lsb;
    uint32_t curr_length = current->data_length_lsb;
    iso9660_dir_record_t *file_record = i9660_find_directory_record(curr_extent, curr_length, filename);

    if (file_record == NULL) {
        printf("File '%s' not found.\n", filename);
        return -1;
    }

    if (file_record->file_flags & 0x02) {
        printf("'%s' is a directory, not a file.\n", filename);
        return -1;
    }
    
    uint32_t file_length = file_record->data_length_lsb;
    if (buffer_size < file_length) {
        printf("Buffer too small: file size %u, buffer size %u.\n", file_length, buffer_size);
        return -1;
    }
    
    uint32_t sectors = (file_length + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    uint32_t extent = file_record->extent_lba_lsb;
    for (uint32_t i = 0; i < sectors; i++) {
        i9660_read_cd_sector(extent + i, buffer + (i * ISO_SECTOR_SIZE));
    }

    return file_length;
}
