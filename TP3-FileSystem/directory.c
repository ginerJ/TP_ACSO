#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
    struct inode dir_inode;

    if (inode_iget(fs, dirinumber, &dir_inode) < 0 ||
        !(dir_inode.i_mode & IALLOC) ||
        ((dir_inode.i_mode & IFMT) != IFDIR)) {
        return -1;
    }

    int dir_size = inode_getsize(&dir_inode);
    int num_entries = dir_size / sizeof(struct direntv6);
    char buf[DISKIMG_SECTOR_SIZE];

    for (int block = 0, entry_index = 0; entry_index < num_entries; ++block) {
        int bytes_read = file_getblock(fs, dirinumber, block, buf);
        if (bytes_read <= 0) break;

        struct direntv6 *entries = (struct direntv6 *)buf;
        for (int i = 0; i < bytes_read / sizeof(struct direntv6); ++i) {
            if (entries[i].d_inumber == 0) continue;
            if (strncmp(entries[i].d_name, name, sizeof(entries[i].d_name)) == 0) {
                *dirEnt = entries[i];
                return 0;
            }
            ++entry_index;
        }
    }
    return -1;
}
