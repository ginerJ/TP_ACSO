#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!pathname || pathname[0] != '/') return -1;

    int current_inode = 1;
    char path_buffer[256];
    strncpy(path_buffer, pathname, sizeof(path_buffer));
    path_buffer[sizeof(path_buffer) - 1] = '\0';

    for (char *directory_name = strtok(path_buffer, "/"); directory_name != NULL; directory_name = strtok(NULL, "/")) {
        struct direntv6 directory_entry;
        if (directory_findname(fs, directory_name, current_inode, &directory_entry) < 0) return -1;
        current_inode = directory_entry.d_inumber;
    }
    return current_inode;
}
