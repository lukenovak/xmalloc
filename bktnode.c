#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "bktnode.h"
#include "slabmalloc.h"

bktnode* make_bktnode(size_t size, bktnode** prevptr, bktnode* next) {
    bktnode* head = slabmalloc(1);    
    head->next = NULL;
    head->prevptr = prevptr;
    head->next = next;
    head->size = size;
    memset(&head->used, 255, 8*sizeof(int));
    return head;
}

void remove(bktnode* node) {
    *node->prevptr = node->next;
    if(node->next != NULL) {
        node->next->prevptr = node->prevptr;
    }
}

void* get_chunk(bktnode* node) {
    for(int i = 0; 8*sizeof(int)*node->size*i < 4096-sizeof(bktnode); i++) { //for each in used
        if(node->used[i] != 0) { //if all bits are not unset
            int j = __builtin_ffs(node->used[i])-1;
            if ((8*sizeof(int)*i + j + 1) * node->size > 4096-sizeof(bktnode)) {
                return NULL;
            }
            int mask = 1 << j; //make a mask for that bit
            node->used[i] ^= mask; //unset it
            //return the associated chunk
            return (void*)node + sizeof(bktnode) + node->size*(8*i*sizeof(int) + j);
        }
    }
    return NULL;
}

void insert(bktnode* node) {
    node->next = *node->prevptr;
    *node->prevptr = node;
    if (node->next != NULL) {
        node->next->prevptr = &node->next;
    }
}

void free_chunk(bktnode* node, void* item) {
    //get which chunk item is in
    int chunknum = (item-(void*)node-sizeof(bktnode))/node->size;
    //set the appropriate bit in used
    __sync_fetch_and_or(&node->used[chunknum/(8*sizeof(int))], 1<<(chunknum % (8*sizeof(int))));
}
