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

void getANSfile(char *diskImg, char *file, char* name);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <disk image> <file>\n", argv[0]);
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

    int size = getDiskSize(pointer)+getFreeSize(getDiskSize(pointer), pointer);
    if(size > 0){
        int fileOpen = open(argv[2], O_RDWR);
        if(fileOpen < 0){
            printf("Error: failed to read file\n");
            close(diskImg);
            exit(1);
        }
        char* secondpointer = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, fileOpen, 0);
        if (secondpointer == MAP_FAILED) {
            printf("Error: failed to map memory\n");
            exit(1);
        }
        getANSfile(pointer, secondpointer, argv[2]);
        munmap(secondpointer, buffer.st_size);
        close(fileOpen);
    }

    munmap(pointer, buffer.st_size);
	close(diskImg);
    return 0;
}

/*
    
*/
void getANSfile(char* diskImg, char* file, char* name) {
    	while (pointer[0] != 0x00) {
		if ((pointer[11] & 0b00000010) == 0 && (pointer[11] & 0b00001000) == 0) {
			char* currentFileName = malloc(sizeof(char));
			char* currentFileExtension = malloc(sizeof(char));
			for (int i = 0; i < 8; i++) {
				if (p[i] == ' ') {
					break;
				}
				currentFileName[i] = p[i];
			}
			for (i = 0; i < 3; i++) {
				currentFileExtension[i] = p[i+8];
			}

			strcat(currentFileName, ".");
			strcat(currentFileName, currentFileExtension);

			if (strcmp(Name, currentFileName) == 0) {
				return (p[26]) + (p[27] << 8);
			}

		}
		p += 32;
	}
	return -1;
}
