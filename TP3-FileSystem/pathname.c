#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!pathname || pathname[0] != '/') return -1;

    int curr_inumber = 1;
    char path_copy[256];
    strncpy(path_copy, pathname, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';

    for (char *token = strtok(path_copy, "/"); token != NULL; token = strtok(NULL, "/")) {
        struct direntv6 entry;
        if (directory_findname(fs, token, curr_inumber, &entry) < 0) return -1;
        curr_inumber = entry.d_inumber;
    }
    return curr_inumber;
}
