#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256
#define NUM_PAGES 256
#define TLB_SIZE 16

int getPageNum(int logicalAddr);
int getOffset(int logicalAddr);
int tlbLookup(int page);
void tlbInsert(int page, int frame);
void tlbInvalidate(int page);
int getFreeFrame(void);
int selectVictimFrame(void);
void loadPageIntoFrame(int page, int frame); 
int handlePageFault(int page);
void initState(void);
void writeStatistics(FILE *out);

signed char memory[256*256];
int pageTable[256];
int nextFreeFrame = 0;
FILE *backingStore;
int totalRefs = 0;
int pageFaults = 0;

typedef struct {
    int physicalAddr;
    signed char physicalMemoryByte;
} MemoryItem;
MemoryItem translate(int logicalAddr);

int main(int argc, char *argv[]) {
    //ensures command has proper arguments
    if (argc != 4 
       || ((strcmp(argv[1], "256") != 0 || strcmp(argv[1], "128") != 0) 
       && strcmp(argv[2], "BACKING_STORE.bin") != 0
       && strcmp(argv[3], "addresses.txt") != 0)
    ) {
        printf("Invalid command\n");
        printf("Instead run either of the following 2 commands:\n");
        printf("%s 256 BACKING_STORE.bin addresses.txt\n", argv[0]);
        printf("%s 128 BACKING_STORE.bin addresses.txt\n", argv[0]);
        return 1;
    }

    int frameCount = atoi(argv[1]);     //either 256 or 128
    backingStore = fopen(argv[2], "rb");
    FILE *addresses = fopen(argv[3], "r");
    int addr;

    //create csv file (either output256.csv or output128.csv)
    char fileName[32];
    sprintf(fileName, "output%d.csv", frameCount);
    FILE *output = fopen(fileName, "w");   //opens output file for writing

    initState();
    
    while (fscanf(addresses, "%d", &addr) == 1){
        addr = addr & 0xFFFF;               //mask address to 16 bit int
        MemoryItem item = translate(addr);  //translates logical addr to phyisical addr

        //writes into output file
        fprintf(output, "%d,%d,%d\n", addr, item.physicalAddr, item.physicalMemoryByte);
    }

    fclose(backingStore);
    fclose(addresses);
    fclose(output);
    return 0;
}

// Page num and Offset Helpers
int getPageNum(int logicalAddr){
    return logicalAddr >> 8;
}

int getOffset(int logicalAddr){
    return logicalAddr & 0xFF;
}

// TLB
int tlbLookup(int page){
    return 0;
}

void tlbInsert(int page, int frame){
    return;
}

void tlbInvalidate(int page){
    return;
}

// Frames/Replacement 
int getFreeFrame(void){
    return 0;
}
int selectVictimFrame(void){
    return 0;
}

// Paging
void loadPageIntoFrame(int page, int frame){
    int offset = page * 256;
    int destination = frame * 256;

    fseek(backingStore, offset, SEEK_SET);
    size_t bytes = fread(memory + destination, 1, 256, backingStore);

    if (bytes != 256){
        fprintf(stderr, "Could not read 256 bytes from BACKING_STORE.bin");
        exit(1);
    }

    return;
}

int handlePageFault(int page){
    return 0;
}

// Translation
MemoryItem translate(int logicalAddr){
    int pageNum = getPageNum(logicalAddr);
    int offset = getOffset(logicalAddr);

    int frame = pageTable[pageNum];
    if (frame == -1){                       // page fault
        pageFaults++;
        frame = nextFreeFrame++;
        loadPageIntoFrame(pageNum, frame);
        pageTable[pageNum] = frame;
    }

    int physicalAddr = frame * PAGE_SIZE + offset;
    signed char physicalMemoryByte = memory[physicalAddr];
    totalRefs++;

    return (MemoryItem){physicalAddr, physicalMemoryByte};
}

// Output
void initState(void){ //resets page table (-1 = entry not in memory)
    for (int i = 0; i < 256; i++){
        pageTable[i] = -1;
    }
    return;
}

void writeStatistics(FILE *out){
    return;
}