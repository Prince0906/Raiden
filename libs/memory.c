#include "memory.h"
#include "config.h"
#include <stdlib.h>   /* malloc — one-time pool claim, allowed by rules */
#include <stdio.h>    /* printf — for fatal error messages              */

/*
 * Pool layout: contiguous sequence of [BlockHeader | aligned data bytes].
 * Every allocation is preceded by a BlockHeader so my_dealloc can locate
 * it with a simple pointer subtraction — no separate metadata table needed.
 */
typedef struct {
    int size;     /* byte count of the data area (not including header) */
    int is_free;  /* 0 = in use, 1 = available for reuse               */
} BlockHeader;

#define HEADER_SIZE  ((int)sizeof(BlockHeader))

/* Round n up to the nearest multiple of 4 for alignment safety. */
#define ALIGN4(n)    (((n) + 3) & ~3)

static char *pool      = 0;
static int   pool_used = 0;   /* byte offset of the next free region    */

/* ── mem_init ────────────────────────────────────────────────────────── */
void mem_init(void) {
    pool = (char *)malloc(POOL_SIZE_BYTES);
    if (!pool) {
        printf("FATAL: memory pool allocation failed\n");
        exit(1);
    }
    pool_used = 0;
}

/* ── my_alloc ────────────────────────────────────────────────────────── */
/*
 * First tries to reuse a freed block (first-fit search). If none is
 * suitable, bump-allocates from the unused tail of the pool.
 * This makes V2's bullet spawn / free cycle zero-waste.
 */
void *my_alloc(int size) {
    if (!pool) { printf("FATAL: mem_init not called\n"); exit(1); }

    int aligned = ALIGN4(size);

    /* First-fit search through previously bumped blocks */
    int offset = 0;
    while (offset + HEADER_SIZE <= pool_used) {
        BlockHeader *hdr = (BlockHeader *)(pool + offset);
        if (hdr->is_free && hdr->size >= aligned) {
            hdr->is_free = 0;
            return (void *)(pool + offset + HEADER_SIZE);
        }
        offset += HEADER_SIZE + hdr->size;
    }

    /* No reusable block — extend the bump pointer */
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

/* ── my_dealloc ──────────────────────────────────────────────────────── */
/*
 * The header is always sizeof(BlockHeader) bytes before the data pointer,
 * so we find it with a single subtraction. No metadata table required.
 */
void my_dealloc(void *ptr) {
    if (!ptr) return;
    BlockHeader *hdr = (BlockHeader *)((char *)ptr - HEADER_SIZE);
    hdr->is_free = 1;
}
