#include <stdio.h>
#include <stdlib.h>
#include "../mymalloc.h"

int main(void) {
   void *p = malloc(64);
    if (!p) {
        printf("FAIL: malloc failed unexpectedly\n");
        return 1;
    }

    // Intentionally do NOT free(p)
    printf("DONE: exiting without freeing one allocation; leak checker should report it\n");
    return 0;    
}