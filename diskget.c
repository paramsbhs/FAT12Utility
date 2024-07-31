#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void get_file(const char *disk_image, const char *file_name);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <disk image> <file name>\n", argv[0]);
        return 1;
    }

    const char *disk_image = argv[1];
    const char *file_name = argv[2];

    get_file(disk_image, file_name);

    return 0;
}


void get_file(const char *disk_image, const char *file_name) {
    // TODO
}
