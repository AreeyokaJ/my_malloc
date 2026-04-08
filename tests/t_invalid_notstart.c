#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    char *p = malloc(32);
    if (!p) {
        printf("FAIL: malloc failed unexpectedly\n");
        return 1;
    }

    free(p + 1);          // invalid free: not the start of the payload

    // Cleanup attempt (optional): if your allocator rejects p+1 without corrupting state,
    // freeing the real pointer should still be valid.
    free(p);

    printf("DONE: check stderr for invalid free message (not-start)\n");
    return 0;   

}