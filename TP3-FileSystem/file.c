#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inode_number, int block_number, void *buffer) {
    struct inode inode_data;

    if (inode_iget(fs, inode_number, &inode_data) < 0) return -1;

    int disk_block_number = inode_indexlookup(fs, &inode_data, block_number);
    if (disk_block_number < 0) return -1;

    if (diskimg_readsector(fs->dfd, disk_block_number, buffer) != DISKIMG_SECTOR_SIZE) return -1;

    int file_size = inode_getsize(&inode_data);
    int byte_offset = block_number * DISKIMG_SECTOR_SIZE;

    return (file_size > byte_offset) ?
           ((file_size - byte_offset > DISKIMG_SECTOR_SIZE) ? DISKIMG_SECTOR_SIZE : file_size - byte_offset) : 0;
}
