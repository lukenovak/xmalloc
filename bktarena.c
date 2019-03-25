#include <stdlib.h>
#include <unistd.h>

#include "bktalloc.h"
#include "bktnode.h"

bktarena* make_bktalloc(void* start, int arena) {
    memset(start, 0, sizeof(bktarena));
    bktarena* allocer = (bktarena*) start;
    allocer->arena = arena;
    allocer->mutex = pthread_mutex_init(&allocer->mutex, NULL);
    return start;
}

int find_bucket(size_t size) {
    int count = 0;
    while(size != 0) 
    { 
        size >>= 1; 
        count += 1; 
    } 

    if (size && !(size & (size - 1))) {
        return count-6;
    } else {
        return count-5;
    }
}

void* bktmalloc(size_t size, bktarena* allocer) {
    int bucketnum = find_bucket(size);
    if(allocer->buckets[bucketnum] == NULL) {
        allocer->buckets[bucketnum] = make_bktnode(1 << (bucketnum + 4), 
                &allocer->buckets[bucketnum], allocer->arena);
    }
    return get_chunk(allocer->buckets[bucketnum]);
}

void bktfree(bktnode* node, void* item) {
    free_chunk(node, item);
}
