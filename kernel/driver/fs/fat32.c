#include "driver/fs/fat32.h"

#include "driver/diskio.h"
#include "lib/malloc.h"
#include "lib/path.h"
#include "lib/stdio.h"
#include "lib/stdlib.h"
#include "lib/string.h"
#include "shell.h"

fat32_boot_sector_t *fat32_boot_sector;
uint8_t buffer[512];

void fat32_read_boot_sector(void) {
    ata_read_sector(0, buffer);
    fat32_boot_sector = (fat32_boot_sector_t *)buffer;
}

uint32_t fat32_first_data_sector(void) {
    return fat32_boot_sector->BPB_RsvdSecCnt +
           (fat32_boot_sector->BPB_NumFATs * fat32_boot_sector->BPB_FATSz32);
}

uint32_t fat32_cluster_to_sector(uint32_t cluster) {
    return fat32_first_data_sector() +
           ((cluster - 2) * fat32_boot_sector->BPB_SecPerClus);
}

void fat32_read_cluster(uint32_t cluster, uint8_t *buffer) {
    uint32_t first_sector = fat32_cluster_to_sector(cluster);
    uint32_t sectors_per_cluster = fat32_boot_sector->BPB_SecPerClus;
    uint32_t bytes_per_sector = fat32_boot_sector->BPB_BytsPerSec;
    for (uint32_t i = 0; i < sectors_per_cluster; i++) {
        ata_read_sector(first_sector + i, buffer + (i * bytes_per_sector));
    }
}

void fat32_write_cluster(uint32_t cluster, const uint8_t *buffer) {
    uint32_t first_sector = fat32_cluster_to_sector(cluster);
    uint32_t sectors_per_cluster = fat32_boot_sector->BPB_SecPerClus;
    uint32_t bytes_per_sector = fat32_boot_sector->BPB_BytsPerSec;
    for (uint32_t i = 0; i < sectors_per_cluster; i++) {
        ata_write_sector(first_sector + i,
                         (uint8_t *)(buffer + (i * bytes_per_sector)));
    }
}

uint32_t fat32_get_next_cluster(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;  // 4 bytes per FAT entry
    uint32_t fat_sector = fat32_boot_sector->BPB_RsvdSecCnt +
                          (fat_offset / fat32_boot_sector->BPB_BytsPerSec);
    uint32_t ent_offset = fat_offset % fat32_boot_sector->BPB_BytsPerSec;

    uint8_t sector_buffer[512];
    ata_read_sector(fat_sector, sector_buffer);

    uint32_t next_cluster = *(uint32_t *)(sector_buffer + ent_offset);
    next_cluster &= 0x0FFFFFFF;  // Mask to 28 bits.
    return next_cluster;
}

int fat32_read_file(uint32_t start_cluster, uint8_t *buffer,
                    uint32_t file_size) {
    uint32_t cluster = start_cluster;
    uint32_t bytes_per_cluster =
        fat32_boot_sector->BPB_SecPerClus * fat32_boot_sector->BPB_BytsPerSec;
    uint32_t bytes_read = 0;

    while (bytes_read < file_size &&
           cluster < 0x0FFFFFF8) {  // 0x0FFFFFF8 marks end-of-chain.
        uint8_t cluster_buffer[bytes_per_cluster];
        fat32_read_cluster(cluster, cluster_buffer);

        uint32_t copy_size = (file_size - bytes_read < bytes_per_cluster)
                                 ? (file_size - bytes_read)
                                 : bytes_per_cluster;
        memcpy(buffer + bytes_read, cluster_buffer, copy_size);
        bytes_read += copy_size;

        if (bytes_read < file_size) {
            cluster = fat32_get_next_cluster(cluster);
            if (cluster >= 0x0FFFFFF8) {
                break;  // End-of-chain reached.
            }
        }
    }
    return bytes_read;
}

// Convert a raw 11-byte FAT filename into a null-terminated string.
void fat32_format_filename(char *dest, const uint8_t *raw_name) {
    int i, j = 0;
    // base name, 8 chars + skip whitespaces
    for (i = 0; i < 8 && raw_name[i] != ' '; i++) dest[j++] = raw_name[i];

    // extension, 3 chars
    int ext_len = 0;
    for (i = 8; i < 11 && raw_name[i] != ' '; i++) { ext_len++; }
    if (ext_len > 0) {
        dest[j++] = '.';
        for (i = 8; i < 11 && raw_name[i] != ' '; i++) {
            dest[j++] = raw_name[i];
        }
    }
    dest[j] = '\0';
}

// case-insensitive string comparison.
int fat32_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1, c2 = *s2;
        if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
        if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

uint32_t fat32_get_first_cluster_from_entry(fat32_dir_entry_t *entry) {
    return (entry->DIR_FstClusHI << 16) | entry->DIR_FstClusLO;
}

int fat32_find_entry(uint32_t dir_cluster, const char *name,
                     fat32_dir_entry_t *entry_out) {
    uint32_t sectors_per_cluster = fat32_boot_sector->BPB_SecPerClus;
    uint32_t bytes_per_sector = fat32_boot_sector->BPB_BytsPerSec;
    uint32_t cluster_size = sectors_per_cluster * bytes_per_sector;
    uint8_t *cluster_buffer = (uint8_t *)malloc(cluster_size);
    if (!cluster_buffer) return -1;
    uint32_t entries_per_cluster = cluster_size / 32;

    while (dir_cluster < 0x0FFFFFF8) {
        fat32_read_cluster(dir_cluster, cluster_buffer);
        for (uint32_t i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t *dentry =
                (fat32_dir_entry_t *)(cluster_buffer + i * 32);
            if (dentry->DIR_Name[0] == 0x00) {  // No more entries.
                free(cluster_buffer);
                return -1;
            }
            if (dentry->DIR_Name[0] == 0xE5) continue;  // Deleted entry.
            if (dentry->DIR_Attr == 0x0F) continue;     // Long filename entry.

            char formatted[13];
            fat32_format_filename(formatted, dentry->DIR_Name);
            if (fat32_strcasecmp(formatted, name) == 0) {
                memcpy(entry_out, dentry, sizeof(fat32_dir_entry_t));
                free(cluster_buffer);
                return 0;
            }
        }
        uint32_t next = fat32_get_next_cluster(dir_cluster);
        if (next >= 0x0FFFFFF8) break;
        dir_cluster = next;
    }
    free(cluster_buffer);
    return -1;
}

int fat32_traverse_path(const char *path, fat32_dir_entry_t *result,
                        int is_dir_path, int print_notfound) {
    uint32_t current_cluster = fat32_boot_sector->BPB_RootClus;
    char path_copy[256];

    strncpy(path_copy, path, sizeof(path_copy));

    char *token = strtok(path_copy, "/");
    fat32_dir_entry_t entry;

    while (token != NULL) {
        if (fat32_find_entry(current_cluster, token, &entry) != 0) {
            if (print_notfound) print_err("FAT32: Entry `%s` not found", token);
            return -1;
        }

        token = strtok(NULL, "/");

        if (token != NULL) {
            if (!(entry.DIR_Attr & 0x10)) {
                print_err("FAT32: Not a Directory");
                return -1;
            }
            current_cluster = fat32_get_first_cluster_from_entry(&entry);
        } else {
            if (is_dir_path && !(entry.DIR_Attr & 0x10)) {
                print_err("FAT32: Not a Directory");
                return -1;
            }
            if (result) memcpy(result, &entry, sizeof(fat32_dir_entry_t));
            return 0;
        }
    }

    // simulate root directory entry.
    if (result) {
        memset(result, 0, sizeof(fat32_dir_entry_t));
        result->DIR_Attr = 0x10;  // Mark it as a directory (root directory)
    }

    return 0;
}

int fat32_read_file_from_path(const char *path, uint8_t *buffer,
                              uint32_t buffer_size) {
    fat32_dir_entry_t entry;
    if (fat32_traverse_path(path, &entry, 0, 0) != 0) {
        print_err("FAT32: File not Found");
        return -1;
    }
    if (entry.DIR_Attr & 0x10) {
        print_err("FAT32: Not a File");
        return -1;
    }
    uint32_t file_size = entry.DIR_FileSize;
    if (buffer_size < file_size) file_size = buffer_size;
    uint32_t start_cluster = fat32_get_first_cluster_from_entry(&entry);
    return fat32_read_file(start_cluster, buffer, file_size);
}

int fat32_list_directory(const char *path, fs_entry_t dirs[256]) {
    fat32_dir_entry_t entry;
    uint32_t dir_cluster;

    if (path[0] == '/' && strlen(path) == 1) {
        dir_cluster = fat32_boot_sector->BPB_RootClus;
    } else {
        if (fat32_traverse_path(path, &entry, 1, 0) != 0) {
            print_err("FAT32: Directory Not Found");
            return -1;
        }
        dir_cluster = fat32_get_first_cluster_from_entry(&entry);
    }

    uint32_t sectors_per_cluster = fat32_boot_sector->BPB_SecPerClus;
    uint32_t bytes_per_sector = fat32_boot_sector->BPB_BytsPerSec;
    uint32_t cluster_size = sectors_per_cluster * bytes_per_sector;
    uint8_t *cluster_buffer = (uint8_t *)malloc(cluster_size);
    if (!cluster_buffer) return -1;

    uint32_t entries_per_cluster = cluster_size / 32;
    int entry_count = 0;
    int dir_idx = 0;

    while (dir_cluster < 0x0FFFFFF8) {
        fat32_read_cluster(dir_cluster, cluster_buffer);
        for (uint32_t i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t *dentry =
                (fat32_dir_entry_t *)(cluster_buffer + i * 32);
            if (dentry->DIR_Name[0] == 0x00) break;
            if (dentry->DIR_Name[0] == 0xE5) continue;
            if (dentry->DIR_Attr == 0x0F) continue;
            char formatted[13];
            fat32_format_filename(formatted, dentry->DIR_Name);
            strncpy(dirs[dir_idx].name, formatted, 13);
            dirs[dir_idx].is_dir = dentry->DIR_Attr == 0x10;
            dir_idx++;
            entry_count++;
        }
        uint32_t next_cluster = fat32_get_next_cluster(dir_cluster);
        if (next_cluster >= 0x0FFFFFF8) break;
        dir_cluster = next_cluster;
    }

    free(cluster_buffer);
    return entry_count;
}

uint32_t fat32_get_parent_directory_cluster(const char *path) {
    if (!path || *path != '/') return 0;  // Invalid path
    char parent_path[256];
    get_directory(path, parent_path);
    if (strlen(parent_path) == 1 && parent_path[0] == '/')
        return fat32_boot_sector->BPB_RootClus;  // Root directory cluster
    fat32_dir_entry_t parent_dir;
    if (fat32_traverse_path(parent_path, &parent_dir, 1, 0) == 0)
        return (parent_dir.DIR_FstClusHI << 16) | parent_dir.DIR_FstClusLO;
    return 0;
}

int fat32_write_file_from_path(const char *path, const uint8_t *buffer,
                               uint32_t buffer_size) {
    fat32_dir_entry_t entry;
    if (fat32_traverse_path(path, &entry, 0, 0) != 0) {
        print_err("FAT32: File not Found");
        return -1;
    }
    if (entry.DIR_Attr & 0x10) {
        print_err("FAT32: Not a File");
        return -1;
    }
    uint32_t start_cluster = fat32_get_first_cluster_from_entry(&entry);
    return fat32_write_file(start_cluster, buffer, buffer_size, &entry,
                            fat32_get_parent_directory_cluster(path));
}

int fat32_update_directory_entry(fat32_dir_entry_t *dir_entry,
                                 uint32_t dir_cluster) {
    if (!dir_entry) return -1;

    uint8_t dir_buffer[fat32_boot_sector->BPB_SecPerClus *
                       fat32_boot_sector->BPB_BytsPerSec];
    uint32_t cluster = dir_cluster;

    while (cluster < 0x0FFFFFF8) {
        fat32_read_cluster(cluster, dir_buffer);
        for (int i = 0; i < (fat32_boot_sector->BPB_SecPerClus *
                             fat32_boot_sector->BPB_BytsPerSec) /
                                sizeof(fat32_dir_entry_t);
             i++) {
            fat32_dir_entry_t *entry =
                (fat32_dir_entry_t *)(dir_buffer +
                                      (i * sizeof(fat32_dir_entry_t)));
            // if (strncmp(entry->DIR_Name, dir_entry->DIR_Name, 11) == 0) {
            if (entry->DIR_FstClusHI == dir_entry->DIR_FstClusHI &&
                entry->DIR_FstClusLO == dir_entry->DIR_FstClusLO) {
                memcpy(entry, dir_entry, sizeof(*dir_entry));
                fat32_write_cluster(cluster, dir_buffer);
                return 0;
            }
        }
        cluster = fat32_get_next_cluster(cluster);
    }
    return -1;
}

int fat32_write_file(uint32_t start_cluster, const uint8_t *buffer,
                     uint32_t file_size, fat32_dir_entry_t *dir_entry,
                     uint32_t parent_directory_cluster) {
    uint32_t cluster = start_cluster;
    uint32_t bytes_per_cluster =
        fat32_boot_sector->BPB_SecPerClus * fat32_boot_sector->BPB_BytsPerSec;
    uint32_t bytes_written = 0;

    if (cluster == 0) {
        cluster = fat32_allocate_next_cluster(0);
        if (cluster == 0) return -1;
        fat32_mark_cluster_as_used(cluster);
        dir_entry->DIR_FstClusLO = cluster & 0xFFFF;
        dir_entry->DIR_FstClusHI = (cluster >> 16) & 0xFFFF;
    }

    while (bytes_written < file_size) {
        uint8_t cluster_buffer[bytes_per_cluster];
        uint32_t copy_size = (file_size - bytes_written < bytes_per_cluster)
                                 ? (file_size - bytes_written)
                                 : bytes_per_cluster;
        memcpy(cluster_buffer, buffer + bytes_written, copy_size);

        fat32_write_cluster(cluster, cluster_buffer);

        bytes_written += copy_size;

        if (bytes_written < file_size) {
            uint32_t next_cluster = fat32_allocate_next_cluster(cluster);
            if (next_cluster == 0) return -1;
            fat32_mark_cluster_as_used(next_cluster);
            fat32_set_next_cluster(cluster, next_cluster);
            cluster = next_cluster;
        }
    }

    // Update file size
    dir_entry->DIR_FileSize = file_size;

    if (fat32_update_directory_entry(dir_entry, parent_directory_cluster) < 0) {
        print_err("FAT32: Error updating dir entry");
        return -1;
    }

    return bytes_written;
}

uint32_t fat32_allocate_next_cluster(uint32_t current_cluster) {
    for (uint32_t cluster = 2; cluster < FAT32_MAX_CLUSTERS; cluster++) {
        uint32_t fat_entry = fat32_read_fat_entry(cluster);
        if (fat_entry == 0) {  // free cluster
            fat32_write_fat_entry(cluster, 0x0FFFFFFF);
            return cluster;
        }
    }
    return 0;
}

void fat32_set_next_cluster(uint32_t current_cluster, uint32_t next_cluster) {
    fat32_write_fat_entry(current_cluster, next_cluster);
}

void fat32_mark_cluster_as_used(uint32_t cluster) {
    fat32_write_fat_entry(cluster, 0x0FFFFFFF);
}

uint32_t fat32_read_fat_entry(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_offset / 512;
    uint32_t fat_entry_offset = fat_offset % 512;
    uint8_t fat_buffer[512];
    ata_read_sector(fat_sector, fat_buffer);
    uint32_t fat_entry;
    memcpy(&fat_entry, fat_buffer + fat_entry_offset, 4);
    return fat_entry;
}

void fat32_write_fat_entry(uint32_t cluster, uint32_t value) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_offset / 512;
    uint32_t fat_entry_offset = fat_offset % 512;
    uint8_t fat_buffer[512];
    ata_read_sector(fat_sector, fat_buffer);
    memcpy(fat_buffer + fat_entry_offset, &value, 4);
    ata_write_sector(fat_sector, fat_buffer);
}

int fat32_create_file(const char *path) {
    if (!path || *path != '/') {
        print_err("FAT32: Invalid path");
        return -1;
    }

    if (fat32_traverse_path(path, NULL, 0, 0) == 0) {
        print_err("FAT32: File already exists");
        return -1;
    }

    char parent_path[256];
    get_directory(path, parent_path);

    fat32_dir_entry_t parent_dir;
    if (fat32_traverse_path(parent_path, &parent_dir, 1, 0) != 0) {
        print_err("FAT32: Parent directory not found");
        return -1;
    }

    uint32_t parent_directory_cluster =
        (parent_dir.DIR_FstClusHI << 16) | parent_dir.DIR_FstClusLO;
    if (parent_directory_cluster == 0) {
        parent_directory_cluster = fat32_boot_sector->BPB_RootClus;
    }

    char filename_raw[128] = {0};
    char filename[11] = {0};
    get_filename(path, filename_raw);
    convert_to_fat32(filename_raw, filename);

    uint8_t dir_buffer[fat32_boot_sector->BPB_SecPerClus *
                       fat32_boot_sector->BPB_BytsPerSec];
    uint32_t cluster = parent_directory_cluster;

    while (cluster < 0x0FFFFFF8) {
        fat32_read_cluster(cluster, dir_buffer);

        for (int i = 0; i < (fat32_boot_sector->BPB_SecPerClus *
                             fat32_boot_sector->BPB_BytsPerSec) /
                                sizeof(fat32_dir_entry_t);
             i++) {
            fat32_dir_entry_t *entry =
                (fat32_dir_entry_t *)(dir_buffer +
                                      (i * sizeof(fat32_dir_entry_t)));

            if (entry->DIR_Name[0] == 0x00 || entry->DIR_Name[0] == 0xE5) {
                memset(entry, 0, sizeof(fat32_dir_entry_t));
                memcpy(entry->DIR_Name, filename, 11);
                entry->DIR_Attr = 0x20;
                entry->DIR_FileSize = 0;

                uint32_t first_cluster = fat32_allocate_next_cluster(0);
                if (first_cluster) {
                    fat32_mark_cluster_as_used(first_cluster);
                    entry->DIR_FstClusLO = first_cluster & 0xFFFF;
                    entry->DIR_FstClusHI = (first_cluster >> 16) & 0xFFFF;
                } else {
                    entry->DIR_FstClusLO = 0;
                    entry->DIR_FstClusHI = 0;
                }
                fat32_write_cluster(cluster, dir_buffer);
                return 0;
            }
        }

        // Move to the next directory cluster
        cluster = fat32_get_next_cluster(cluster);
    }

    print_err("FAT32: No space in directory");
    return -1;
}

int fat32_remove_file(const char *path) {
    uint32_t bytes_per_cluster =
        fat32_boot_sector->BPB_SecPerClus * fat32_boot_sector->BPB_BytsPerSec;
    uint8_t empty_cluster[bytes_per_cluster];
    memset(empty_cluster, 0, bytes_per_cluster);

    fat32_dir_entry_t dir_entry;
    int result = fat32_traverse_path(path, &dir_entry, 0, 0);

    if (result != 0) {
        print_err("FAT32: File doesn't exist");
        return -1;
    }

    uint32_t cluster = fat32_get_first_cluster_from_entry(&dir_entry);

    while (cluster >= 2 && cluster < 0x0FFFFFF8) {
        fat32_write_cluster(cluster, empty_cluster);
        uint32_t next_cluster = fat32_get_next_cluster(cluster);
        cluster = next_cluster;
    }

    for (int i = 0; i < 11; i++) dir_entry.DIR_Name[i] = 0;
    dir_entry.DIR_Attr = 0xE5;  // deleted attribute

    uint32_t parent_cluster = fat32_get_parent_directory_cluster(path);
    fat32_update_directory_entry(&dir_entry, parent_cluster);
    return 0;
}

int fat32_create_directory_entry(const char *path, fat32_dir_entry_t *entry,
                                 uint32_t first_cluster) {
    memset(entry, 0, sizeof(fat32_dir_entry_t));

    get_filename(path, (char *)entry->DIR_Name);
    if (strlen(entry->DIR_Name) > 8) return -1;
    for (int i = 0; i < 8; ++i)
        entry->DIR_Name[i] = toupper(entry->DIR_Name[i]);

    entry->DIR_Attr = 0x10;

    entry->DIR_FstClusHI = (first_cluster >> 16) & 0xFFFF;
    entry->DIR_FstClusLO = first_cluster & 0xFFFF;

    entry->DIR_CrtTime = 0;
    entry->DIR_CrtDate = 0;
    entry->DIR_WrtTime = 0;
    entry->DIR_WrtDate = 0;
    entry->DIR_LstAccDate = 0;
    entry->DIR_FileSize = 0;
    entry->DIR_CrtTimeTenth = 0;
    entry->DIR_NTRes = 0;

    return 0;
}

int fat32_find_free_directory_entry(uint32_t cluster) {
    uint8_t buffer[fat32_boot_sector->BPB_BytsPerSec *
                   fat32_boot_sector->BPB_SecPerClus];
    fat32_dir_entry_t *entry;
    uint32_t entries_per_cluster, i;

    fat32_read_cluster(cluster, buffer);

    entries_per_cluster = (fat32_boot_sector->BPB_BytsPerSec *
                           fat32_boot_sector->BPB_SecPerClus) /
                          sizeof(fat32_dir_entry_t);

    for (i = 0; i < entries_per_cluster; ++i) {
        entry = (fat32_dir_entry_t *)(buffer + (i * sizeof(fat32_dir_entry_t)));
        if (entry->DIR_Name[0] == 0 || entry->DIR_Name[0] == 0xE5) return i;
    }

    return -1;
}

int fat32_write_directory_entry(uint32_t cluster, uint32_t offset,
                                fat32_dir_entry_t *entry) {
    uint8_t buffer[fat32_boot_sector->BPB_BytsPerSec *
                   fat32_boot_sector->BPB_SecPerClus];
    fat32_read_cluster(cluster, buffer);
    uint32_t entry_offset = offset * sizeof(fat32_dir_entry_t);
    memcpy(buffer + entry_offset, entry, sizeof(fat32_dir_entry_t));
    fat32_write_cluster(cluster, buffer);
    return 0;
}

int fat32_create_directory(const char *path) {
    fat32_dir_entry_t entry;
    uint32_t parent_cluster;
    uint32_t new_cluster;
    uint32_t dir_entry_offset;
    int result;

    parent_cluster = fat32_get_parent_directory_cluster(path);
    if (parent_cluster == 0) {
        print_err("FAT32: Unable to find parent directory");
        return -1;
    }

    result = fat32_traverse_path(path, &entry, 1, 0);
    if (result == 0) {
        print_err("FAT32: Directory already exists");
        return -2;
    }

    new_cluster = fat32_allocate_next_cluster(0);
    if (new_cluster == 0) {
        print_err("FAT32: No free clusters available");
        return -3;
    }

    fat32_write_fat_entry(new_cluster, 0x0FFFFFFF);

    result = fat32_create_directory_entry(path, &entry, new_cluster);
    if (result != 0) {
        print_err("FAT32: Error creating directory entry");
        return -4;
    }

    dir_entry_offset = fat32_find_free_directory_entry(parent_cluster);
    if (dir_entry_offset == -1) {
        print_err(
            "FAT32: No space for a new directory entry in the parent "
            "directory");
        return -5;
    }

    result =
        fat32_write_directory_entry(parent_cluster, dir_entry_offset, &entry);
    if (result != 0) {
        print_err(
            "FAT32: Error writing directory entry to the parent directory");
        return -6;
    }

    uint8_t cluster_data[fat32_boot_sector->BPB_BytsPerSec *
                         fat32_boot_sector->BPB_SecPerClus];
    memset(cluster_data, 0, sizeof(cluster_data));
    fat32_write_cluster(new_cluster, cluster_data);

    fat32_dir_entry_t dot_entry;
    fat32_create_directory_entry(".", &dot_entry, new_cluster);
    fat32_write_directory_entry(new_cluster, 0, &dot_entry);

    fat32_dir_entry_t dotdot_entry;
    fat32_create_directory_entry("..", &dotdot_entry, parent_cluster);
    fat32_write_directory_entry(new_cluster, 1, &dotdot_entry);

    return 0;
}

fs_info_t fat32_fs_info() {
    return (fs_info_t){
        FAT32,
        fat32_boot_sector->BS_VolLab,
        11,
        (uint64_t)((fat32_boot_sector->BPB_TotSec16 != 0)
                       ? fat32_boot_sector->BPB_TotSec16
                       : fat32_boot_sector->BPB_TotSec32) *
            fat32_boot_sector->BPB_BytsPerSec,
    };
}