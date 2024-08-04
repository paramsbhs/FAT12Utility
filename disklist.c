#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*
    This function calculates the total size of the disk in bytes.
    The disk size is calculated by multiplying the sector size (bytes per sector) by the total number of sectors.
*/
int getDiskSize(char* pointer) {
    int sectorBytes = pointer[11] + (pointer[12] << 8);
    int totalSectors = pointer[19] + (pointer[20] << 8);
    return sectorBytes * totalSectors;
}
/*
    This function calculates the free size of the disk in bytes.
    The free size is calculated by counting the number of free sectors in the FAT and multiplying by the sector size.
*/
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

void listDir(char* pointer);

int main(int argc, char *argv[]) {
  if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk image>\n", argv[0]);
        exit(1);
    }

    int diskimg = open(argv[1], O_RDWR);
	if (diskimg < 0) {
		printf("Error: failed to read disk image\n");
		exit(1);
	}
	struct stat buffer;
	fstat(diskimg, &buffer);
	char* pointer = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, diskimg, 0); //taken from Tutorial
	if (pointer == MAP_FAILED) {
		printf("Error: failed to map memory\n");
		exit(1);
	}

    listDir(pointer + 512 *19); 
    munmap(pointer, buffer.st_size);
	close(diskimg);

    return 0;
}


void listDir(char* pointer) {
    while (pointer[0] != 0x00) {
        char type;
        if ((pointer[11] & 0b00010000) == 0b00010000) {
            type = 'D';
        } else {
            type = 'F';
        }

        // Allocate memory for file name and extension
        char* file = malloc(9); // 8 characters + null terminator
        char* extension = malloc(4); // 3 characters + null terminator

        // Copy file name
        int i = 0;
        while(i < 8){
            if (pointer[i] == ' ') break;
            file[i] = pointer[i];
            i++;
        }
        file[i] = '\0'; // Null-terminate the string

        // Copy file extension
        for (i = 0; i < 3; i++) {
            extension[i] = pointer[i + 8];
        }
        extension[3] = '\0'; // Null-terminate the string

        // Concatenate file name and extension
        strcat(file, ".");
        strcat(file, extension);

        int size = (getDiskSize(pointer)+getFreeSize(getDiskSize(pointer),pointer));

        // Extract date and time information
        int year = ((pointer[17] & 0b11111110) >> 1) + 1980;
        int month = ((pointer[16] & 0b11100000) >> 5) + ((pointer[17] & 0b00000001) << 3);
        int day = pointer[16] & 0b00011111;
        int hour = (pointer[15] & 0b11111000) >> 3;
        int minute = ((pointer[14] & 0b11100000) >> 5) + ((pointer[15] & 0b00000111) << 3);

        // Check if file is not hidden and not system file
        if (!(pointer[11] & 0b00000010) && !(pointer[11] & 0b00001000)) {
            printf("%c %10d %20s %d-%02d-%02d %02d:%02d\n",
                   type, size, file, year, month, day, hour, minute);
        }

        // Free allocated memory
        free(file);
        free(extension);

        // Move to the next entry
        pointer += 32;
    }
}
