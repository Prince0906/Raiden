#include "memory.h"
#include "config.h"
#include <stdlib.h>   
#include <stdio.h>    

typedef struct {
    int size;     
    int is_free;  
} BlockHeader;

#define HEADER_SIZE  ((int)sizeof(BlockHeader))

#define ALIGN4(n)    (((n) + 3) & ~3)

static char *pool      = 0;
static int   pool_used = 0;   

void mem_init(void) {
    pool = (char *)malloc(POOL_SIZE_BYTES);
    if (!pool) {
        printf("FATAL: memory pool allocation failed\n");
        exit(1);
    }
    pool_used = 0;
}

void *my_alloc(int size) {
    if (!pool) { printf("FATAL: mem_init not called\n"); exit(1); }

    int aligned = ALIGN4(size);

    int offset = 0;
    while (offset + HEADER_SIZE <= pool_used) {
        BlockHeader *hdr = (BlockHeader *)(pool + offset);
        if (hdr->is_free && hdr->size >= aligned) {
            hdr->is_free = 0;
            return (void *)(pool + offset + HEADER_SIZE);
        }
        offset += HEADER_SIZE + hdr->size;
    }

    int needed = HEADER_SIZE + aligned;
    if (pool_used + needed > POOL_SIZE_BYTES) {
        printf("FATAL: memory pool exhausted\n");
        exit(1);
    }

    BlockHeader *hdr = (BlockHeader *)(pool + pool_used);
    hdr->size    = aligned;
    hdr->is_free = 0;
    pool_used   += needed;

    return (void *)((char *)hdr + HEADER_SIZE);
}

void my_dealloc(void *ptr) {
    if (!ptr) return;
    BlockHeader *hdr = (BlockHeader *)((char *)ptr - HEADER_SIZE);
    hdr->is_free = 1;
}
