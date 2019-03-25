#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "bktnode.h"
#include "slabmalloc.h"

bktnode* make_bktnode(size_t size, bktnode** prevptr, int arena) {
    bktnode* head = slabmalloc();    
    head->next = NULL;
    head->prevptr = prevptr;
    head->arena = arena;
    head->size = size;
    memset(&head->used, 255, 8*sizeof(int));
    return head;
}

void remove(bktnode* node) {
    *node->prevptr = node->next;
    if(node->next != null) {
        node->next->prevptr = node->prevptr;
    }
}

void* get_chunk(bktnode* node) {
    for(int i = 0; 8*sizeof(int)*i*node->size < 4096-sizeof(bktnode); i++) { //for each in used
        if(node->used[i] != 0) { //if all bits are not unset
            int j = __builtin_ffs(node->used[i])-1;
            int mask << j; //make a mask for that bit
            node->used[i] ^= mask; //set it
            //if that was the last chunk
            if ((8*i + j)*node->size == 4096-sizeof(bktnode)) {
                remove(node); //remove this node from the list
            }
            //return the associated chunk
            return (void*)node + sizeof(bktnode) + node->size*(8*i + j);
        }
    }
}

void insert(bktnode* node) {
    *node->prevptr = node;
    if (node->next != NULL) {
        node->next->prevptr = &node->next;
    }
}

void free_chunk(bktnode* node, void* item) {
    //get which chunk item is in
    int chunknum = (item-(void*)node-sizeof(bktnode))/node->size;
    //unset the appropriate bit in used
    node->used[chunknum/8] ^= 1<<(chunknum % 8);
    //if the previous node isn't pointing to us (i.e. we are not in the list)
    if(*node->prevptr != node) {
        //put us back in the list
        insert(node);
    }
}
