#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define FAT32_MAX_CLUSTERS 268435456

typedef struct {
    // Standard BPB
    uint8_t  BS_jmpBoot[3];      // Jump instruction to boot code
    uint8_t  BS_OEMName[8];      // OEM Name in ASCII
    uint16_t BPB_BytsPerSec;     // Bytes per sector
    uint8_t  BPB_SecPerClus;     // Sectors per cluster
    uint16_t BPB_RsvdSecCnt;     // Reserved sector count
    uint8_t  BPB_NumFATs;        // Number of FATs
    uint16_t BPB_RootEntCnt;     // Root entry count (FAT12/16 only)
    uint16_t BPB_TotSec16;       // Total sectors (if zero, use BPB_TotSec32)
    uint8_t  BPB_Media;          // Media descriptor
    uint16_t BPB_FATSz16;        // FAT size in sectors (FAT12/16 only)
    uint16_t BPB_SecPerTrk;      // Sectors per track
    uint16_t BPB_NumHeads;       // Number of heads
    uint32_t BPB_HiddSec;        // Hidden sectors
    uint32_t BPB_TotSec32;       // Total sectors (if BPB_TotSec16 is zero)

    // FAT32 Extended BPB
    uint32_t BPB_FATSz32;        // FAT size in sectors for FAT32
    uint16_t BPB_ExtFlags;       // Extended flags
    uint16_t BPB_FSVer;          // Filesystem version
    uint32_t BPB_RootClus;       // Cluster number of the root directory start
    uint16_t BPB_FSInfo;         // Filesystem info sector
    uint16_t BPB_BkBootSec;      // Backup boot sector
    uint8_t  BPB_Reserved[12];   // Reserved

    // Boot Code & Signature
    uint8_t  BS_DrvNum;          // Drive number
    uint8_t  BS_Reserved1;       // Reserved
    uint8_t  BS_BootSig;         // Boot signature (0x29 indicates that the following fields are present)
    uint32_t BS_VolID;           // Volume ID serial number
    uint8_t  BS_VolLab[11];      // Volume label in ASCII
    uint8_t  BS_FilSysType[8];   // Filesystem type in ASCII (often "FAT32   ")
} __attribute__((packed)) fat32_boot_sector_t;

extern fat32_boot_sector_t *fat32_boot_sector;

void fat32_read_boot_sector();

void fat32_read_cluster(uint32_t, uint8_t *);
void fat32_write_cluster(uint32_t, const uint8_t *);

uint32_t fat32_get_next_cluster(uint32_t);

int fat32_read_file(uint32_t, uint8_t *, uint32_t);

// FAT directory entry structure (short 8.3 name only)
typedef struct {
    uint8_t  DIR_Name[11];     // (8 chars).(3 chars) filename (padded with spaces)
    uint8_t  DIR_Attr;         // File attributes
    uint8_t  DIR_NTRes;        // Reserved for NT
    uint8_t  DIR_CrtTimeTenth; // Creation time (tenths of second)
    uint16_t DIR_CrtTime;      // Creation time
    uint16_t DIR_CrtDate;      // Creation date
    uint16_t DIR_LstAccDate;   // Last access date
    uint16_t DIR_FstClusHI;    // High word of first cluster number
    uint16_t DIR_WrtTime;      // Last write time
    uint16_t DIR_WrtDate;      // Last write date
    uint16_t DIR_FstClusLO;    // Low word of first cluster number
    uint32_t DIR_FileSize;     // File size in bytes
} __attribute__((packed)) fat32_dir_entry_t;

int fat32_traverse_path(const char *, fat32_dir_entry_t *, int);
uint32_t fat32_get_parent_directory_cluster(const char *);

int fat32_read_file_from_path(const char *, uint8_t *, uint32_t);
int fat32_write_file_from_path(const char *,const uint8_t *, uint32_t);
int fat32_list_directory(const char *, char[256][256]);

int fat32_write_file(uint32_t, const uint8_t *, uint32_t, fat32_dir_entry_t *, uint32_t);
uint32_t fat32_allocate_next_cluster(uint32_t);
void fat32_set_next_cluster(uint32_t, uint32_t);
void fat32_mark_cluster_as_used(uint32_t);
uint32_t fat32_read_fat_entry(uint32_t);
void fat32_write_fat_entry(uint32_t, uint32_t);

int fat32_create_file(const char *); 

#endif // FAT32_H
