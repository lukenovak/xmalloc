#include <stdlib.h>
// Bucket node
// nodes go at the head of pages
#ifndef BUCKET_NODE_H
#define BUCKET_NODE_H

typedef struct bktnode {
    struct bktnode* next;
    struct bktnode** prevptr;
    int arena;
    size_t size;
    int used[8]; 
    //we obviously lose bytes b/c of this but it's pretty 
    //negligible and a lot easier to implement
} bktnode;

bktnode* make_bktnode(size_t chunksize, bktnode** prevptr, int arena); 
void* get_chunk(bktnode* node);
void free_chunk(bktnode* node, void* item);

#endif
