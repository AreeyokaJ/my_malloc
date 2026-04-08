#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    int x = 123;          // stack variable, NOT from malloc
    free(&x);             // invalid free: not in heap / not allocated by mymalloc

    printf("DONE: check stderr for invalid free message (not-malloc)\n");
    return 0;
}