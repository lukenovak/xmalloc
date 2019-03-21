#include <stdlib.h>
// Bucket node
// nodes go at the head of pages
#ifndef BUCKET_NODE_H
#define BUCKET_NODE_H

typedef struct bktnode {
    struct bktnode* next;
    size_t size;
    void free;
    int[];
} bktnode;

bktnode* make_bktnode(size_t blocksize); 

#endif
