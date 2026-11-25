#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//consts
#define PAGE_SIZE 256
#define NUM_PAGES 256
#define TLB_SIZE 16

//structs
typedef struct {    //represents physical memory item
    int physicalAddr;
    signed char physicalMemoryByte;
} MemoryItem;

typedef struct {    //represents single TLB entry
    int page;
    int frame;
} tlbEntry;

//methods
int getPageNum(int logicalAddr);
int getOffset(int logicalAddr);
int tlbLookup(int page);
void tlbInsert(int page, int frame);
void tlbInvalidate(int page);
int getUnusedFrame(void);
int getLRU(void);
void loadPageIntoFrame(int page, int frame); 
int handlePageFault(int page);
MemoryItem translate(int logicalAddr);
void initState(void);
void writeStatistics(FILE *output);

//global variables
signed char memory[256*256];
int pageTable[256];
int nextFreeFrame = 0;
FILE *backingStore;
int totalRefs = 0;
int pageFaults = 0;
tlbEntry tlb[TLB_SIZE]; 
int tlbNext;
int tlbHits;
int frameCount;
int pageInFrame[256];   //tracks which page is in each frame (-1 if empty)
long time;
long lastUsed[256];     //tracks time at which each frame is used

int main(int argc, char *argv[]) {
    //ensures command has proper arguments
    if (argc != 4) {
        printf("Invalid command: Must pass 4 arguments\n");
        return 1;
    }
    
    frameCount = atoi(argv[1]); //must be either 256 or 128
    if (frameCount != 256 && frameCount != 128){    
        printf("Frame count must be either 256 or 128. Instead is %d", frameCount);
        return 1;
    }
    
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
    writeStatistics(output);
    
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
int tlbLookup(int page){    //returns frame of tlb entry where page = page
    for (int i = 0; i < TLB_SIZE; i++){
        if (tlb[i].page == page){
            return tlb[i].frame;
        }
    }
    
    return -1;
}

void tlbInsert(int page, int frame){    //inserts new entry into tlb, overwriting entry in current spot
    tlbEntry insert;
    insert.page = page;
    insert.frame = frame;

    tlb[tlbNext] = insert;
    tlbNext = (tlbNext + 1) % TLB_SIZE;
    
    return;
}

void tlbInvalidate(int page){   //sets given page of tlb to -1
    for (int i = 0; i < TLB_SIZE; i++){
        if (tlb[i].page == page){
            tlb[i].page = -1;
        }
    }
    return;
}

// Frames/Replacement 
int getUnusedFrame(void){     //searches for unused frame and returns frame number 
    for (int i = 0; i < frameCount; i++){
        if (lastUsed[i] == -1){
            nextFreeFrame = i;
            return i;
        }
    }
    return -1;  // returns -1 if all frames have been used already
}
int getLRU(void){    //finds least recently used frame
    long minTime;
    int firstUsedFrame;
    for (int i = 0; i < frameCount; i++){
        if (lastUsed[i] != -1 && lastUsed[i] < minTime){
            minTime = lastUsed[i];
            firstUsedFrame = i;
        }
    }
    return firstUsedFrame;
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

int handlePageFault(int page){    //places page in unused frame
    int frame = getUnusedFrame();

    //if all frames are in use, clear LRU frame 
    if (frame == -1){
        frame = getLRU();
        int oldPage = pageInFrame[frame];
        tlbInvalidate(oldPage);
        pageTable[oldPage] = -1;
    }
    loadPageIntoFrame(page, frame);
    pageTable[page] = frame;
    pageInFrame[frame] = page;
    return frame;
}

// Translation
MemoryItem translate(int logicalAddr){
    int pageNum = getPageNum(logicalAddr);
    int offset = getOffset(logicalAddr);

    int frame = tlbLookup(pageNum);
    if (frame != -1){
        tlbHits++;
    }
    else{
        frame = pageTable[pageNum];
        if (frame == -1){
            pageFaults++;
            frame = handlePageFault(pageNum);
        }
        tlbInsert(pageNum, frame);
    }
    time++;
    lastUsed[frame] = time;
        
    int physicalAddr = (frame * PAGE_SIZE) + offset;
    signed char physicalMemoryByte = memory[physicalAddr];
    totalRefs++;

    return (MemoryItem){physicalAddr, physicalMemoryByte};
}

// Output
void initState(void){ //resets tlb and trackers (-1 = no entry)
    for (int i = 0; i < 256; i++){
        pageTable[i] = -1;
        pageInFrame[i] = -1;
        lastUsed[i] = -1;
    }
    tlbNext = 0;
    time = 0;
    nextFreeFrame = 0;

    for (int i = 0; i < TLB_SIZE; i++){
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }

    return;
}

void writeStatistics(FILE *output){     //prints page fault and hit rates to csv file
    double pageFaultRate = 100.0 * pageFaults/totalRefs;
    double hitRate = 100.0 * tlbHits/totalRefs;
    fprintf(output, "Page Faults Rate,%.2f%%,\n", pageFaultRate);
    fprintf(output, "TLB Hits Rate,%.2f%%,", hitRate);
    return;
}