#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void put_file(const char *diskimg, const char *file, const char *dest_dir);

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s <diskimg> <file> \n", argv[0]);
        return 1;
    }

    int diskImg = open(argv[1], O_RDWR); //Opens the disk image
    if (diskImg < 0) { //Error handling
        printf("Error: failed to read disk image\n");
        exit(1);
    }

    struct stat buffer; //Taken from Tutorial
    fstat(diskImg, &buffer);
    char* pointer = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, diskImg, 0);
    if (pointer == MAP_FAILED) {
        printf("Error: failed to map memory\n");
        exit(1);
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
