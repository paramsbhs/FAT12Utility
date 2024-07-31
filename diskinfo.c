#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void print_os_name(const char *disk_image);
void print_label(const char *disk_image);
void print_total_size(const char *disk_image);
void print_free_size(const char *disk_image);
void print_file_count(const char *disk_image);
void print_fat_info(const char *disk_image);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk image>\n", argv[0]);
        return 1;
    }

    const char *disk_image = argv[1];

    print_os_name(disk_image);
    print_label(disk_image);
    print_total_size(disk_image);
    print_free_size(disk_image);
    print_file_count(disk_image);
    print_fat_info(disk_image);

    return 0;
}


void print_os_name(const char *disk_image) {
    // TODO
}

void print_label(const char *disk_image) {
    // TODO
}

void print_total_size(const char *disk_image) {
    // TODO
}

void print_free_size(const char *disk_image) {
    // TODO
}

void print_file_count(const char *disk_image) {
    // TODO
}

void print_fat_info(const char *disk_image) {
    // TODO
}
