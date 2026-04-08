#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
    void *p = malloc(16);
    if (!p) {
        printf("FAIL: malloc failed unexpectedly\n");
        return 1;
    }

    free(p);              // valid
    free(p);              // invalid: double free

    return 0;
}