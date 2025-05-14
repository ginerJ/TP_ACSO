#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inode_ptr) {
    if (inumber < 1) return -1;

    int inodes_per_sector = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int sector_number = (inumber - 1) / inodes_per_sector + INODE_START_SECTOR;
    int inode_index = (inumber - 1) % inodes_per_sector;

    struct inode inode_buffer[inodes_per_sector];
    if (diskimg_readsector(fs->dfd, sector_number, inode_buffer) != DISKIMG_SECTOR_SIZE) return -1;

    *inode_ptr = inode_buffer[inode_index];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inode_ptr, int block_num) {
    if (!(inode_ptr->i_mode & IALLOC) || block_num < 0) return -1;

    int pointers_per_block = DISKIMG_SECTOR_SIZE / sizeof(unsigned short);

    if (!(inode_ptr->i_mode & ILARG))
        return (block_num >= 8 || inode_ptr->i_addr[block_num] == 0) ? -1 : inode_ptr->i_addr[block_num];

    int direct_block_limit = 7 * pointers_per_block;

    if (block_num < direct_block_limit) {
        int indirect_block_num = inode_ptr->i_addr[block_num / pointers_per_block];
        if (indirect_block_num == 0) return -1;

        unsigned short indirect_pointers[pointers_per_block];
        if (diskimg_readsector(fs->dfd, indirect_block_num, indirect_pointers) != DISKIMG_SECTOR_SIZE) return -1;

        int data_block_num = indirect_pointers[block_num % pointers_per_block];
        return data_block_num == 0 ? -1 : data_block_num;
    }

    int double_indirect_index = block_num - direct_block_limit;
    unsigned short first_level_pointers[pointers_per_block];

    if (diskimg_readsector(fs->dfd, inode_ptr->i_addr[7], first_level_pointers) != DISKIMG_SECTOR_SIZE) return -1;

    int second_level_block_num = first_level_pointers[double_indirect_index / pointers_per_block];
    if (second_level_block_num == 0) return -1;

    unsigned short second_level_pointers[pointers_per_block];
    if (diskimg_readsector(fs->dfd, second_level_block_num, second_level_pointers) != DISKIMG_SECTOR_SIZE) return -1;

    return (second_level_pointers[double_indirect_index % pointers_per_block] == 0) ? -1 : second_level_pointers[double_indirect_index % pointers_per_block];
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1);
}
