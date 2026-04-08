#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "mymalloc.h"

#define RUNS 50
#define N 120

static long elapsed_us(struct timeval start, struct timeval end) {
    long sec  = (long)(end.tv_sec - start.tv_sec);
    long usec = (long)(end.tv_usec - start.tv_usec);
    return sec * 1000000L + usec;
}

static void task1(void) {
    for (int i = 0; i < N; i++) {
        void *p = malloc(1);
        free(p);
    }
}
static void task2(void) {
    void *ptrs[N];

    for (int i = 0; i < N; i++) {
        ptrs[i] = malloc(1);
    }

    for (int i = 0; i < N; i++) {
        free(ptrs[i]);
    }
}

static void task3(void) { 

    void *ptrs[N];
    int live = 0;         // how many are currently allocated (non-NULL)
    int allocs = 0;       // how many total allocations we've successfully made

    for (int i = 0; i < N; i++) ptrs[i] = NULL;

    while (allocs < N) {
        int do_alloc = rand() & 1; // 0 or 1

        if (do_alloc) {
            if (live < N) {
                // find first empty slot
                int idx = 0;
                while (idx < N && ptrs[idx] != NULL) idx++;

                if (idx < N) {
                    ptrs[idx] = malloc(1);
                    if (ptrs[idx] != NULL) {
                        live++;
                        allocs++;
                    }
                }
            }
        } else {
            if (live > 0) {
                // free a random live pointer
                int target = rand() % live;

                int seen = 0;
                for (int i = 0; i < N; i++) {
                    if (ptrs[i] != NULL) {
                        if (seen == target) {
                            free(ptrs[i]);
                            ptrs[i] = NULL;
                            live--;
                            break;
                        }
                        seen++;
                    }
                }
            }
        }
    }


    // free any remaining allocations
    for (int i = 0; i < N; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

}

typedef struct node {
    struct node *next;
    int value;
} node;

static void task4(void) { 
    node *head = NULL;

    //build list of N nodes
    for (int i = 0; i < N; i++) {
        node *n = (node *)malloc(sizeof(node));
        if (!n) break;              // if allocator fails, stop building
        n->value = i;
        n->next = head;
        head = n;
    }

    // delete every other node 
    node *cur = head;
    int toggle = 0;
    while (cur && cur->next) {
        toggle = !toggle;
        if (toggle) {
            node *victim = cur->next;       
            cur->next = victim->next;       
            free(victim);                   
        } else {
            cur = cur->next;                
        }
    }

    // rebuild back to N nodes 
    int count = 0;
    for (node *p = head; p != NULL; p = p->next) count++;

    while (count < N) {
        node *n = (node *)malloc(sizeof(node));
        if (!n) break;
        n->value = count;
        n->next = head;
        head = n;
        count++;
    }

    // free the entire list (no leaks)
    cur = head;
    while (cur) {
        node *next = cur->next;
        free(cur);
        cur = next;
    }
}

typedef struct tnode {
    struct tnode *left;
    struct tnode *right;
    int value;
} tnode;

static void task5(void) {
    tnode *nodes[N];

    // allocate N nodes
    for (int i = 0; i < N; i++) {
        nodes[i] = (tnode *)malloc(sizeof(tnode));
        if (!nodes[i]) break;
        nodes[i]->left = NULL;
        nodes[i]->right = NULL;
        nodes[i]->value = i;
    }

    // Phase B: link into a complete binary tree shape using indices
    for (int i = 0; i < N; i++) {
        if (!nodes[i]) break;
        int li = 2 * i + 1;
        int ri = 2 * i + 2;
        nodes[i]->left  = (li < N) ? nodes[li] : NULL;
        nodes[i]->right = (ri < N) ? nodes[ri] : NULL;
    }

    // free  leaves first, then parents
    // Leaves are roughly indices N/2 .. N-1 in an array-backed complete tree.
    for (int i = N / 2; i < N; i++) {
        if (nodes[i]) { free(nodes[i]); nodes[i] = NULL; }
    }
    for (int i = (N / 2) - 1; i >= 0; i--) {
        if (nodes[i]) { free(nodes[i]); nodes[i] = NULL; }
    }

    // allocate again to force reuse of freed blocks, then free all
    for (int i = 0; i < N; i++) {
        nodes[i] = (tnode *)malloc(sizeof(tnode));
        if (!nodes[i]) break;
        nodes[i]->left = nodes[i]->right = NULL;
        nodes[i]->value = i;
    }

    for (int i = 0; i < N; i++) {
        if (nodes[i]) { free(nodes[i]); nodes[i] = NULL; }
    }
}


static void workload_once(void) {
    task1();
    task2();
    task3();
    task4();
    task5();
}

int main(void) {
    // fixed seed = reproducible runs
    srand(214);

    struct timeval start, end;
    long total_us = 0;

    for (int i = 0; i < RUNS; i++) {
        gettimeofday(&start, NULL);
        workload_once();
        gettimeofday(&end, NULL);
        total_us += elapsed_us(start, end);
    }

    double avg_us = (double)total_us / (double)RUNS;
    printf("Average time over %d runs: %.2f microseconds\n", RUNS, avg_us);

    return 0;
}