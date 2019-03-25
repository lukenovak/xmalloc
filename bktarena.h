#include "bktnode.h"
#ifndef BUCKET_ARENA
#define BUCKET_ARENA

typedef struct bktarena {
    bktnode* buckets[7];
    int arena;
    pthread_mutex_t mutex;
} bktarena;

bktarena* make_bktarena(void* start, int arena);
void* bktmalloc(size_t size, bktarena* allocer);
void bktfree(bktnode* node, void* item);
#endif
