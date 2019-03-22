#include <stdlib.h>
// Bucket node
// nodes go at the head of pages
#ifndef BUCKET_NODE_H
#define BUCKET_NODE_H

typedef struct bktnode {
    struct bktnode* next;
    struct bktnode** prevptr;
    size_t size;
    void[31] used;
} bktnode;

bktnode* make_bktnode(size_t blocksize); 
void* get_chunk(bktnode* node);
void free_chunk(void* item);

#endif
