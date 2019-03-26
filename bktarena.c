#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "xmalloc.h"
#include "bktnode.h"
#include "bktarena.h"

bktarena* make_bktarena(void* start, int arena) {
    memset(start, 0, sizeof(bktarena));
    bktarena* allocer = (bktarena*) start;
    allocer->arena = arena;
    pthread_mutex_init(&allocer->mutex, 0); 
    return start;
}

int find_bucket(size_t size) {
    for (int i = 4; 1 << i <= 1024; i++) {
        if (size <= 1 << i) {
            return i-4;
        }
    }
    return -1;
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
