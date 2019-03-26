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
    if (size < 16) {
        return 0;
    }
    else {
        int count = 0;
        while(size != 0) 
        { 
            size >>= 1; 
            count += 1; 
        } 

        if (size && size != 1 << (count-1)) {
            return count-5;
        } else {
            return count-4;
        }
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
