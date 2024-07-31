#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void list_directory(const char *disk_image, int logical_cluster);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk image>\n", argv[0]);
        return 1;
    }

    const char *disk_image = argv[1];

    list_directory(disk_image, 0); 

    return 0;
}


void list_directory(const char *disk_image, int logical_cluster) {
    // TODO
}
