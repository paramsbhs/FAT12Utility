#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void put_file(const char *disk_image, const char *file_path, const char *dest_dir);

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s <disk image> <file path> [<destination directory>]\n", argv[0]);
        return 1;
    }

    const char *disk_image = argv[1];
    const char *file_path = argv[2];
    const char *dest_dir = (argc == 4) ? argv[3] : "/";

    put_file(disk_image, file_path, dest_dir);

    return 0;
}

void put_file(const char *disk_image, const char *file_path, const char *dest_dir) {
    // TODO
}
