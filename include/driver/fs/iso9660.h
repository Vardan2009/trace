#ifndef ISO9660_H
#define ISO9660_H

#include <stdint.h>
#include "lib/fs.h"

#define ATA_SECTOR_SIZE      512
#define ISO_SECTOR_SIZE      2048
#define SECTORS_PER_ISO      (ISO_SECTOR_SIZE / ATA_SECTOR_SIZE)  // 4

// CD-ROM's system area occupies the first 32 KiB,
// and the volume descriptors start at sector 16 (2048-byte sectors)
// which corresponds to LBA: 16 * (2048 / 512) = 64.
#define PVD_LBA              (16 * SECTORS_PER_ISO)

typedef struct {
    /* 0–7 */
    uint8_t  type;                      // Offset 0: Volume Descriptor Type (should be 1 for PVD)
    char     id[5];                     // Offset 1–5: Standard Identifier ("CD001")
    uint8_t  version;                   // Offset 6: Volume Descriptor Version (should be 1)
    uint8_t  unused1;                   // Offset 7: Unused (typically 0)

    /* 8–71 */
    char     sys_id[32];                // Offset 8–39: System Identifier
    char     vol_id[32];                // Offset 40–71: Volume Identifier

    /* 72–87 */
    uint8_t  unused2[8];                // Offset 72–79: Unused Field (all zeroes)
    uint32_t vol_space_size_lsb;        // Offset 80–83: Volume Space Size (LSB part, little-endian)
    uint32_t vol_space_size_msb;        // Offset 84–87: Volume Space Size (MSB part, big-endian ignored on x86)

    /* 88–139 */
    uint8_t  unused3[32];               // Offset 88–119: Unused Field
    int16_t  vol_set_size_lsb;          // Offset 120–121: Volume Set Size (LSB part)
    int16_t  vol_set_size_msb;          // Offset 122–123: Volume Set Size (MSB part)
    int16_t  vol_seq_num_lsb;           // Offset 124–125: Volume Sequence Number (LSB part)
    int16_t  vol_seq_num_msb;           // Offset 126–127: Volume Sequence Number (MSB part)
    int16_t  logical_block_sz_lsb;      // Offset 128–129: Logical Block Size (LSB part)
    int16_t  logical_block_sz_msb;      // Offset 130–131: Logical Block Size (MSB part)
    uint32_t path_table_sz_lsb;         // Offset 132–135: Path Table Size (LSB part)
    uint32_t path_table_sz_msb;         // Offset 136–139: Path Table Size (MSB part)

    /* 140–155: Path Table Locations */
    uint32_t l_path_table;              // Offset 140–143: Location of Type-L Path Table (little-endian)
    uint32_t opt_l_path_table;          // Offset 144–147: Location of Optional Type-L Path Table (little-endian)
    uint32_t m_path_table;              // Offset 148–151: Location of Type-M Path Table (big-endian)
    uint32_t opt_m_path_table;          // Offset 152–155: Location of Optional Type-M Path Table (big-endian)

    /* 156–189: Root Directory Record */
    uint8_t  root_dir[34];              // Offset 156–189: Directory Record for the root directory

    /* 190–701: Identifiers */
    char     vol_set_id[128];           // Offset 190–317: Volume Set Identifier (strD)
    char     publisher_id[128];         // Offset 318–445: Publisher Identifier (strA)
    char     data_preparer_id[128];     // Offset 446–573: Data Preparer Identifier (strA)
    char     application_id[128];       // Offset 574–701: Application Identifier (strA)

    /* 702–812: File Identifier Strings */
    char     copyright_file_id[37];     // Offset 702–738: Copyright File Identifier (strD)
    char     abstract_file_id[37];      // Offset 739–775: Abstract File Identifier (strD)
    char     bibliographic_file_id[37]; // Offset 776–812: Bibliographic File Identifier (strD)

    /* 813–880: Date and Time fields (in dec-datetime format) */
    char     vol_creation_date[17];     // Offset 813–829: Volume Creation Date and Time
    char     vol_mod_date[17];          // Offset 830–846: Volume Modification Date and Time
    char     vol_expiration_date[17];   // Offset 847–863: Volume Expiration Date and Time
    char     vol_effective_date[17];    // Offset 864–880: Volume Effective Date and Time

    /* 881–882 */
    uint8_t  file_structure_version;    // Offset 881: File Structure Version (should be 1)
    uint8_t  unused4;                   // Offset 882: Unused (0)

    /* 883–1394 */
    uint8_t  app_use[512];              // Offset 883–1394: Application Use (contents not defined by ISO 9660)

    /* 1395–2047 */
    uint8_t  reserved[653];             // Offset 1395–2047: Reserved (should be zero)
} __attribute__((packed)) i9660_pvd_t;

typedef struct {
    uint8_t length;
    uint8_t ext_attr_length;
    uint32_t extent_lba_lsb;
    uint32_t extent_lba_msb;  // ignored
    uint32_t data_length_lsb;
    uint32_t data_length_msb; // ignored
    uint8_t recording_date[7];
    uint8_t file_flags;
    uint8_t file_unit_size;
    uint8_t interleave_gap_size;
    uint16_t vol_seq_number_lsb;
    uint16_t vol_seq_number_msb; // ignored
    uint8_t file_id_length;
    // Followed by file identifier and optional padding.
} __attribute__((packed)) iso9660_dir_record_t;

typedef void (*dir_record_callback_t)(iso9660_dir_record_t *record);

extern i9660_pvd_t pvd;

void i9660_read_cd_sector(uint32_t cd_sector, uint8_t *buffer);
void i9660_process_directory(uint32_t extent, uint32_t length, dir_record_callback_t callback);
void i9660_get_file_identifier(iso9660_dir_record_t *record, char *dest, int dest_size);
int i9660_compare_file_identifier(iso9660_dir_record_t *record, const char *name);
iso9660_dir_record_t *i9660_find_directory_record(uint32_t extent, uint32_t length, const char *name);
iso9660_dir_record_t *i9660_dir_from_path(const char *path) ;
int i9660_read_directory(const char *path, fs_entry_t dirs[256]);
int i9660_read_file_from_path(const char *path, uint8_t *buffer, uint32_t buffer_size);
void i9660_read_pvd(i9660_pvd_t *);

fs_info_t i9660_fs_info();

#endif // ISO9660_H