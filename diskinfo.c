#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void getOSName(char* disk_image, char* pointer);
void getlabel(char* disk_image, char* pointer);
int getDiskSize(char *pointer);
int getFreeSize(int disksize, char* pointer);
int rootfiles(char* pointer);
int getNumFatCopies(char* pointer);
int getSectorsPerFat(char* pointer);
void printAllInfo(char* disk_image, char *diskLabel, int diskSize, int freeSize, int numberOfRootFiles, int numberOfFatCopies, int sectorsPerFat);

int main(int argc, char* argv[]) {
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

    char* oslabel = malloc(sizeof(char));
    char* disklabel = malloc(sizeof(char));
    getOSName(oslabel, pointer);
    getlabel(disklabel, pointer);
    int disksize = getDiskSize(pointer);
    int freesize = getFreeSize(disksize, pointer);
    int rootFiles = rootfiles(pointer);
    int fatCopies = getNumFatCopies(pointer);
    int sectors = getSectorsPerFat(pointer);
    printAllInfo(oslabel, disklabel, disksize, freesize, rootFiles, fatCopies, sectors);

    free(oslabel);
	free(disklabel);    
    munmap(pointer, buffer.st_size);
	close(diskimg);
    return 0;
}


void getOSName(char* oslabel, char* pointer) {
    for(int i = 0; i < 8; i++){
        oslabel[i] = pointer[i+3];
    }
}

void getlabel(char* diskLabel, char* pointer) {
    int i;
	for (i = 0; i < 8; i++) {
		diskLabel[i] = pointer[i+43];
	}

	if (diskLabel[0] == ' ') {
		pointer += 512 * 19;
		while (pointer[0] != 0x00) {
			if (pointer[11] == 8) {
				for (i = 0; i < 8; i++) {
					diskLabel[i] = pointer[i];
				}
			}
			pointer += 32;
		}
	}
}

int getDiskSize(char* pointer) {
	int sectorbytes = pointer[11] + (pointer[12] << 8);
	int total = pointer[19] + (pointer[20] << 8);
	return sectorbytes * total;
}

int getFreeSize(int disksize, char* pointer) {
    int total = 0;
    int size = 512;
    for (int i = 2; i < (disksize/size); i++) {
        int first,second;
        int value;
        if((i%2)==0){
            first = pointer[512 + (3*i/2) + 1] & 0x0F;
            second = pointer[512 + (3*i/2)] & 0xFF;
            value = (first << 8) + second;
        }else{
            first = pointer[512 + (3*i/2)] & 0xF0;
            second = pointer[512 + (3*i/2) + 1] & 0xFF;
            value = (first >> 4) + (second << 4);
        }
        if(value == 0x00) {
            total++;
        }
    }
    return size*total;
}

int rootfiles(char* pointer){
    pointer += 512 * 19;
    int total = 0;
    while(pointer[0] != 0x00){
        if(pointer[11] == 0x08 || pointer[11] == 0x10){
            total++;
        }
        pointer += 32;
    }
    return total;
}

int getNumFatCopies(char* pointer) {
    return pointer[16];
}

int getSectorsPerFat(char* pointer) {
    return pointer[22] + (pointer[23] << 8);
}

void printAllInfo(char* disk_image, char* diskLabel, int diskSize, int freeSize, int numberOfRootFiles, int numberOfFatCopies, int sectorsPerFat) {
	printf("OS Name: %s\n", disk_image);
	printf("Label of the disk: %s\n", diskLabel);
	printf("Total size of the disk: %d\n", diskSize);
	printf("Free size of the disk: %d\n", freeSize);
	printf("==================\n");
	printf("The number of files in the root directory\n(including all files in the root directory and all files in all subdirectories): %d\n", numberOfRootFiles);
	printf("==================\n");
	printf("Number of FAT copies: %d\n", numberOfFatCopies);
	printf("Sectors per FAT: %d\n", sectorsPerFat);
}