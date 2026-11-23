#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 256
#define NUM_PAGES 256
#define TLB_SIZE 16

/* ---------- State ---------- */
// TODO: physical memory (signed char array)
// TODO: page_table[NUM_PAGES]      (page -> frame, -1 if not loaded)
// TODO: frame_to_page[]            (frame -> page, -1 if free)
// TODO: last_used[]                (per frame, for LRU)
// TODO: TLB array of {page, frame} + FIFO insertion index
// TODO: num_frames, next_free_frame, clock
// TODO: counters: total_refs, page_faults, tlb_hits
// TODO: FILE *backing_store

/* ---------- Helpers ---------- */
int get_page_number(int logical_addr);   // mask to 16 bits, shift right 8
int get_offset(int logical_addr);        // low 8 bits

/* ---------- TLB ---------- */
int  tlb_lookup(int page);               // frame or -1; must NOT change FIFO order
void tlb_insert(int page, int frame);    // FIFO overwrite, advance pointer with wraparound
void tlb_invalidate(int page);           // clear the entry if present (needed in Phase 2)

/* ---------- Frames / replacement ---------- */
int  get_free_frame(void);               // next unused frame or -1 if memory is full
int  select_victim_frame(void);          // LRU: frame with smallest last_used

/* ---------- Paging ---------- */
void load_page_into_frame(int page, int frame);  // fseek, fread 256 bytes, check result
int  handle_page_fault(int page);        // free frame or evict; update page table; return frame

/* ---------- Translation ---------- */
int  translate(int logical_addr, signed char *value_out);
// extract page/offset -> TLB -> page table -> fault if needed
// -> TLB insert on miss -> update last_used on EVERY path
// -> compute physical address -> read signed byte -> update counters

/* ---------- Output ---------- */
void init_state(void);
void write_statistics(FILE *out);        // %.2f, match the format at the end of correct256.csv

int main(int argc, char *argv[]) {
    // TODO: check argc == 4
    if (argc != 4){
        return 0;
    }
    // TODO: num_frames = atoi(argv[1]); open argv[2] ("rb") and argv[3] ("r")
    // TODO: build output filename (output256.csv / output128.csv)
    // TODO: init_state(); loop over addresses; translate; write one CSV row each
    // TODO: write_statistics(); close everything
    return 0;
}

int get_page_number(int logical_addr){
    return 0;
} 
int get_offset(int logical_addr){
    return 0;
}    

int  tlb_lookup(int page){
    return 0;
}               
void tlb_insert(int page, int frame){
    return 0;
}     
void tlb_invalidate(int page){
    return 0;
}   

int  get_free_frame(void){
    return 0;
}       
int  select_victim_frame(void){
    return 0;
}   

void load_page_into_frame(int page, int frame){
    return 0;
}   
int  handle_page_fault(int page){
    return 0;
}   

int  translate(int logical_addr, signed char *value_out){
    return 0;
}   

void init_state(void){
    return 0;
}   
void write_statistics(FILE *out){
    return 0;
}   