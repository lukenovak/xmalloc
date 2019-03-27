#include "bktnode.h"
#ifndef BUCKET_ARENA_H
#define BUCKET_ARENA_H

typedef struct bktarena {
    bktnode* buckets[8];
} bktarena;

int find_bucket(size_t size);
void* bktmalloc(size_t size, bktarena* allocer);
void bktfree(bktnode* node, void* item);
#endif
