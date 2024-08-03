#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


void list_directory(char* pointer);

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


void list_directory(char* pointer) {
    while (pointer[0] != 0x00) {
        char fileType;
        if ((pointer[11] & 0b00010000) == 0b00010000) {
            fileType = 'D';
        } else {
            fileType = 'F';
        }

        // Allocate memory for file name and extension
        char* fileName = malloc(9); // 8 characters + null terminator
        char* fileExtension = malloc(4); // 3 characters + null terminator

        // Copy file name
        int i;
        for (i = 0; i < 8; i++) {
            if (pointer[i] == ' ') break;
            fileName[i] = pointer[i];
        }
        fileName[i] = '\0'; // Null-terminate the string

        // Copy file extension
        for (i = 0; i < 3; i++) {
            fileExtension[i] = pointer[i + 8];
        }
        fileExtension[3] = '\0'; // Null-terminate the string

        // Concatenate file name and extension
        strcat(fileName, ".");
        strcat(fileName, fileExtension);

        int fileSize = getFileSize(fileName, pointer);

        // Extract date and time information
        int year = ((pointer[17] & 0b11111110) >> 1) + 1980;
        int month = ((pointer[16] & 0b11100000) >> 5) + ((pointer[17] & 0b00000001) << 3);
        int day = pointer[16] & 0b00011111;
        int hour = (pointer[15] & 0b11111000) >> 3;
        int minute = ((pointer[14] & 0b11100000) >> 5) + ((pointer[15] & 0b00000111) << 3);

        // Check if file is not hidden and not system file
        if (!(pointer[11] & 0b00000010) && !(pointer[11] & 0b00001000)) {
            printf("%c %10d %20s %d-%02d-%02d %02d:%02d\n",
                   fileType, fileSize, fileName, year, month, day, hour, minute);
        }

        // Free allocated memory
        free(fileName);
        free(fileExtension);

        // Move to the next entry
        pointer += 32;
    }
}
