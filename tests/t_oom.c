#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    // Request more than the heap can ever satisfy (even before headers/alignment).
    void *p = malloc(5000);

    if (p != NULL) {
        printf("FAIL: expected NULL for out-of-memory, got non-NULL\n");
        return 1;
    }

    printf("PASS: OOM returned NULL (check stderr for the mymalloc error line)\n");
    return 0;
}