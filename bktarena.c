#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "xmalloc.h"
#include "bktnode.h"
#include "bktarena.h"

int find_bucket(size_t size) {
    for (int i = 4; 1 << i <= 2048; i++) {
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
                &allocer->buckets[bucketnum], NULL);
    }
    bktnode* node = allocer->buckets[bucketnum];
    void* ptr = NULL;
    while(node != NULL) {
        ptr = get_chunk(allocer->buckets[bucketnum]);
        if (ptr != NULL) {
            return ptr;
        }
        node = node->next;
    }
    allocer->buckets[bucketnum] = make_bktnode(1 << (bucketnum + 4), 
            &allocer->buckets[bucketnum], allocer->buckets[bucketnum]);
    return get_chunk(allocer->buckets[bucketnum]);
}

void bktfree(bktnode* node, void* item) {
    free_chunk(node, item);
}
