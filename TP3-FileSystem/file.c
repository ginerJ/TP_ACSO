#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;

    if (inode_iget(fs, inumber, &in) < 0) return -1;

    int diskBlock = inode_indexlookup(fs, &in, blockNum);
    if (diskBlock < 0) return -1;

    if (diskimg_readsector(fs->dfd, diskBlock, buf) != DISKIMG_SECTOR_SIZE) return -1;

    int filesize = inode_getsize(&in);
    int offset = blockNum * DISKIMG_SECTOR_SIZE;
    return (filesize > offset) ?
           ((filesize - offset > DISKIMG_SECTOR_SIZE) ? DISKIMG_SECTOR_SIZE : filesize - offset) : 0;
}
