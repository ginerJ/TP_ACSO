#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1) return -1;

    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int inode_block = (inumber - 1) / inodes_per_block + INODE_START_SECTOR;
    int inode_offset = (inumber - 1) % inodes_per_block;

    struct inode buf[inodes_per_block];
    if (diskimg_readsector(fs->dfd, inode_block, buf) != DISKIMG_SECTOR_SIZE) return -1;

    *inp = buf[inode_offset];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (!(inp->i_mode & IALLOC) || blockNum < 0) return -1;

    int ptrs_per_block = DISKIMG_SECTOR_SIZE / sizeof(unsigned short);

    if (!(inp->i_mode & ILARG)) {
        return (blockNum >= 8 || inp->i_addr[blockNum] == 0) ? -1 : inp->i_addr[blockNum];
    }

    int simple_limit = 7 * ptrs_per_block;

    if (blockNum < simple_limit) {
        int indir_block_num = inp->i_addr[blockNum / ptrs_per_block];
        if (indir_block_num == 0) return -1;

        unsigned short ptrs[ptrs_per_block];
        if (diskimg_readsector(fs->dfd, indir_block_num, ptrs) != DISKIMG_SECTOR_SIZE) return -1;

        int data_block_num = ptrs[blockNum % ptrs_per_block];
        return data_block_num == 0 ? -1 : data_block_num;
    }

    int double_blockNum = blockNum - simple_limit;
    unsigned short first_level[ptrs_per_block];

    if (diskimg_readsector(fs->dfd, inp->i_addr[7], first_level) != DISKIMG_SECTOR_SIZE) return -1;

    int second_indir_block = first_level[double_blockNum / ptrs_per_block];
    if (second_indir_block == 0) return -1;

    unsigned short second_level[ptrs_per_block];
    if (diskimg_readsector(fs->dfd, second_indir_block, second_level) != DISKIMG_SECTOR_SIZE) return -1;

    return (second_level[double_blockNum % ptrs_per_block] == 0) ? -1 : second_level[double_blockNum % ptrs_per_block];
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1);
}
