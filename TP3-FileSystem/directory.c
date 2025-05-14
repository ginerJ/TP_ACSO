#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *target_name, int directory_inode_number, struct direntv6 *directory_entry) {
    struct inode directory_inode;

    if (inode_iget(fs, directory_inode_number, &directory_inode) < 0 ||
        !(directory_inode.i_mode & IALLOC) ||
        ((directory_inode.i_mode & IFMT) != IFDIR)) {
        return -1;
    }

    int directory_size = inode_getsize(&directory_inode);
    int total_entries = directory_size / sizeof(struct direntv6);
    char sector_buffer[DISKIMG_SECTOR_SIZE];

    for (int block_number = 0, current_entry_index = 0; current_entry_index < total_entries; ++block_number) {
        int bytes_read = file_getblock(fs, directory_inode_number, block_number, sector_buffer);
        if (bytes_read <= 0) break;

        struct direntv6 *block_entries = (struct direntv6 *)sector_buffer;
        for (int entry_offset = 0; entry_offset < bytes_read / sizeof(struct direntv6); ++entry_offset) {
            if (block_entries[entry_offset].d_inumber == 0) continue;

            if (strncmp(block_entries[entry_offset].d_name, target_name, sizeof(block_entries[entry_offset].d_name)) == 0) {
                *directory_entry = block_entries[entry_offset];
                return 0;
            }
            ++current_entry_index;
        }
    }
    return -1;
}
