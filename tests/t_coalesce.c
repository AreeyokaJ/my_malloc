#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    
    // Make four allocations that should end up adjacent in the heap and fill up the entiretiy of memory.
    void *a = malloc(1016);
    void *b = malloc(1016);
    void *c = malloc(1016);
    void *d = malloc(1016);

    if (!a || !b || !c || !d) {
        printf("FAIL: initial allocations failed\n");
        return 1;
    }

    // Free both a and b c and d; a correct allocator should coalesce them into one larger free chunk.
    free(a);
    free(b);
    free(c);
    free(d);
    

    // This request is intentionally larger than either single 1016 chunk since those are the whole memory is filled with only those block sizes,
    // so it should only succeed if a+b+c+d got merged.
    void *e = malloc(2032);

    if (!e) {
        printf("FAIL: expected coalescing to allow a larger allocation after freeing adjacent blocks\n");
        return 1;
    }


    printf("PASS: coalescing works (able to allocate larger block after freeing adjacent blocks)\n");
    return 0;
}