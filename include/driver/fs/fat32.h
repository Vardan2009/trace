#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

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

#endif // FAT32_H