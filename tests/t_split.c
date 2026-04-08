#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    void *a = malloc(1);   // tiny alloc; should split the initial free chunk
    void *b = malloc(1);   // should succeed if split left a remainder free chunk

    if (!a || !b) {
        printf("FAIL: expected two small allocations to succeed (split likely broken)\n");
        return 1;
    }

    if (a == b) {
        printf("FAIL: allocations returned same pointer\n");
        return 1;
    }


    free(a);
    free(b);

    printf("PASS: split behavior looks good (two small mallocs succeeded)\n");
    return 0;
}