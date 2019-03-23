#include "bktnode.h"
#ifndef BUCKET_ALLOCATOR
#define BUCKET_ALLOCATOR

typedef struct bktalloc {
    bktnode*[7] buckets;
    int arena;
    pthread_mutex_t mutex;
} bktalloc;

bktalloc* make_bktalloc(void* start);
void* bktmalloc(size_t size, bktalloc* allocer);
void bktfree(bktnode* node, void* item);
#endif
