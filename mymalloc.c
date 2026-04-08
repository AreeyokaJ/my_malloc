#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "mymalloc.h"
#define MEMSIZE 4096
#define ALIGN8(x) (((x) + 7) & ~((size_t)7))
#define HDRSIZE (sizeof(size_t))
#define MINCHUNK ((size_t)16)
#define ALLOC_BIT ((size_t)1)
#define GET_SIZE(hdr) ((hdr) & ~((size_t)0x7))
#define IS_ALLOC(hdr) ((hdr) & (ALLOC_BIT))
#define NEXT_CHUNK(cur) ((cur) + GET_SIZE((*(size_t *)(cur))))



static union {
    char bytes[MEMSIZE];
    double align;
} heap;

// Initialization flag
static int initialized = 0;

// Forward declarations
static void init_heap(void);
static void leak_check(void);

void *mymalloc(size_t size, char *file, int line) {
    if (!initialized) {
        init_heap();
    }
    
    //calculate bytes needed 
    size_t payload = ALIGN8(size);
    size_t need = payload + HDRSIZE;
    
    if (need < MINCHUNK) need = MINCHUNK;

    char *cur = heap.bytes;
    char *end = heap.bytes + MEMSIZE; 

    while (cur < end){
        size_t header = *(size_t *) cur;
        size_t chunk_size = header & ~ALLOC_BIT;
        int allocated = (header & ALLOC_BIT) != 0;

        //if there is space release it 
        if (!allocated && chunk_size >= need){
            size_t extra_space = chunk_size - need;

            if (extra_space >= MINCHUNK){
                size_t *new_header = (size_t *) cur;
                
                *new_header = need | ALLOC_BIT;

                
                size_t *leftover_header = (size_t *)(cur + need);
                *leftover_header = extra_space;

             } else {
                size_t *new_header = (size_t *) cur; 
                *new_header = chunk_size | ALLOC_BIT;
             
            }
            return (void*)(cur + HDRSIZE);
        }

        cur += chunk_size;
    }
    
    fprintf(stderr, "mymalloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    
    return NULL;
}


static char *coalesce_prev(char *cur) {
    char *start = heap.bytes;
    char *end   = heap.bytes + MEMSIZE;

    if (cur == start) return cur;

    char *walker = start;
    char *prev = NULL;

    while (walker < end) {
        size_t wh = *(size_t *)walker;
        size_t wsize = wh & ~ALLOC_BIT;
        char *next = walker + wsize;

        if (next == cur) { prev = walker; break; }
        if (next > cur)  { break; }   // safety if cur isn't on a boundary
        walker = next;
    }

    if (!prev) return cur;

    size_t ph = *(size_t *)prev;
    if (ph & ALLOC_BIT) return cur;   // prev allocated

    size_t prev_size = ph & ~ALLOC_BIT;
    size_t cur_size  = (*(size_t *)cur) & ~ALLOC_BIT;

    *(size_t *)prev = prev_size + cur_size;  // merged free
    return prev;
}

static void coalesce_next(char *cur) {
    char *end = heap.bytes + MEMSIZE;

    size_t h = *(size_t *)cur;
    size_t cur_size = h & ~ALLOC_BIT;

    char *next = cur + cur_size;
    if (next >= end) return;

    size_t nh = *(size_t *)next;
    if (nh & ALLOC_BIT) return;       // next allocated

    size_t next_size = nh & ~ALLOC_BIT;
    *(size_t *)cur = cur_size + next_size;   // merged free
}

static char *coalesce(char *cur) {
    cur = coalesce_prev(cur);
    coalesce_next(cur);
    return cur;
}

void myfree(void *ptr, char *file, int line) {
    if (!initialized) init_heap(); 

    if (ptr == NULL) return; 

    char *p = (char *) ptr;
    char *heap_start = heap.bytes;
    char *heap_end = heap.bytes + MEMSIZE;

    //check to see if it is a valid range within a heap
    if (p < heap_start || p >= heap_end){
        fprintf(stderr, "myfree: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    char *cur = heap.bytes;
    char *end = heap.bytes + MEMSIZE;

    while (cur < end){
        size_t h = *(size_t*)cur;
        size_t chunk_size = h & ~ALLOC_BIT;
        int allocated = h & ALLOC_BIT;

        char *payload = cur + HDRSIZE;

        if ((char*)ptr == payload){

            //check if the chunk is already freed
            if (!allocated){
                fprintf(stderr, "myfree: Inappropriate pointer (%s:%d)\n", file, line);
                exit(2);
            }

            //mark current chunk free 
            *(size_t *)cur = chunk_size;
            
            //coalesce with next and previous chunks if possible 
            cur = coalesce(cur);
            
            //we succesfully freed and coalesced therefore we don't need to 
            return;
        }

        cur += chunk_size;
    }
    
    //if return doesnt happen in the while loop then it was because the pointer passed was inappropriate
    fprintf(stderr, "myfree: Inappropriate pointer (%s:%d)\n", file, line);
    exit(2);     

}

static void init_heap(void) {
    size_t *header = (size_t *) heap.bytes;
    *header = (size_t)MEMSIZE;
    initialized = 1;
    
    atexit(leak_check);

}

static void leak_check(void) {
    char *cur = heap.bytes;
    char *end = heap.bytes + MEMSIZE;
    size_t leaked_bytes = 0; 
    int leaked_objs = 0; 

    while (cur < end){
        size_t hdr = *(size_t *) cur; 
        size_t chunk_size = GET_SIZE(hdr);

        //safety check to prevent infitie loop or if heap is corrupted
        if (chunk_size < MINCHUNK || cur + chunk_size > end){
            break;
        }

        if (IS_ALLOC(hdr)){
            leaked_objs += 1;
            leaked_bytes += chunk_size - HDRSIZE;
        }
        cur = NEXT_CHUNK(cur);
    }

    if (leaked_objs > 0){
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", leaked_bytes, leaked_objs);
    }

}

