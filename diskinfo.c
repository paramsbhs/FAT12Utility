#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void getOSName(char* oslabel, char* pointer);
void getLabel(char* diskLabel, char* pointer);
int getDiskSize(char* pointer);
int getFreeSize(int diskSize, char* pointer);
int rootFiles(char* pointer);
int getNumFatCopies(char* pointer);
int getSectorsPerFat(char* pointer);
void printAllInfo(char* osLabel, char* diskLabel, int diskSize, int freeSize, int numberOfRootFiles, int numberOfFatCopies, int sectorsPerFat);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk image>\n", argv[0]);
        exit(1);
    }

    int diskImg = open(argv[1], O_RDWR);
    if (diskImg < 0) {
        printf("Error: failed to read disk image\n");
        exit(1);
    }

    struct stat buffer;
    fstat(diskImg, &buffer);
    char* pointer = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, diskImg, 0);
    if (pointer == MAP_FAILED) {
        printf("Error: failed to map memory\n");
        exit(1);
    }

    char* osLabel = malloc(9); // 8 characters + null terminator
    char* diskLabel = malloc(9);
    if (osLabel == NULL || diskLabel == NULL) {
        printf("Error: failed to allocate memory\n");
        exit(1);
    }

    getOSName(osLabel, pointer);
    getLabel(diskLabel, pointer);
    int diskSize = getDiskSize(pointer);
    int freeSize = getFreeSize(diskSize, pointer);
    int rootFilesCount = rootFiles(pointer);
    int fatCopies = getNumFatCopies(pointer);
    int sectorsPerFat = getSectorsPerFat(pointer);

    printAllInfo(osLabel, diskLabel, diskSize, freeSize, rootFilesCount, fatCopies, sectorsPerFat);

    free(osLabel);
    free(diskLabel);
    munmap(pointer, buffer.st_size);
    close(diskImg);
    
    return 0;
}

void getOSName(char* osLabel, char* pointer) {
    for (int i = 0; i < 8; i++) {
        osLabel[i] = pointer[i + 3];
    }
    osLabel[8] = '\0';
}

void getLabel(char* diskLabel, char* pointer) {
    for (int i = 0; i < 8; i++) {
        diskLabel[i] = pointer[i + 43];
    }
    diskLabel[8] = '\0';

    if (diskLabel[0] == ' ') {
        pointer += 512 * 19;
        while (pointer[0] != 0x00) {
            if (pointer[11] == 8) {
                for (int i = 0; i < 8; i++) {
                    diskLabel[i] = pointer[i];
                }
                diskLabel[8] = '\0';
                return;
            }
            pointer += 32;
        }
    }
}

int getDiskSize(char* pointer) {
    int sectorBytes = pointer[11] + (pointer[12] << 8);
    int totalSectors = pointer[19] + (pointer[20] << 8);
    return sectorBytes * totalSectors;
}

int getFreeSize(int diskSize, char* pointer) {
    int totalFreeSectors = 0;
    int sectorSize = 512;

    for (int i = 2; i < (diskSize / sectorSize); i++) {
        int firstByte, secondByte, value;

        if ((i % 2) == 0) {
            firstByte = pointer[512 + (3 * i / 2) + 1] & 0x0F;
            secondByte = pointer[512 + (3 * i / 2)] & 0xFF;
            value = (firstByte << 8) + secondByte;
        } else {
            firstByte = pointer[512 + (3 * i / 2)] & 0xF0;
            secondByte = pointer[512 + (3 * i / 2) + 1] & 0xFF;
            value = (firstByte >> 4) + (secondByte << 4);
        }

        if (value == 0x00) {
            totalFreeSectors++;
        }
    }

    return sectorSize * totalFreeSectors;
}

int rootFiles(char* pointer) {
    pointer += 512 * 19;
    int totalFiles = 0;

    while (pointer[0] != 0x00) {
        if (pointer[11] == 0x08 || pointer[11] == 0x10) {
            totalFiles++;
        }
        pointer += 32;
    }

    return totalFiles;
}

int getNumFatCopies(char* pointer) {
    return pointer[16];
}

int getSectorsPerFat(char* pointer) {
    return pointer[22] + (pointer[23] << 8);
}

void printAllInfo(char* osLabel, char* diskLabel, int diskSize, int freeSize, int numberOfRootFiles, int numberOfFatCopies, int sectorsPerFat) {
    printf("OS Name: %s\n", osLabel);
    printf("Label of the disk: %s\n", diskLabel);
    printf("Total size of the disk: %d bytes\n", diskSize);
    printf("Free size of the disk: %d bytes\n", freeSize);
    printf("Number of files in the root directory (including hidden files): %d\n", numberOfRootFiles);
    printf("Number of FAT copies: %d\n", numberOfFatCopies);
    printf("Number of sectors per FAT: %d\n", sectorsPerFat);
}
