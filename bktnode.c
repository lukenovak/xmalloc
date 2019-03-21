#include <stdlib.h>
#include <unistd.h>

#include "bktnode.h"

bktnode* make_bktnode(size_t size, bktnode* next) {
    bktnode* head = mmap(0, 4096, PROT_READ | PROT WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    
    head->next = next;
    head->size = size;
    head->free = 1;
    // size of this array is given by a formula we wrote down somewhere
    head->chunks = int[4079/(size * 8 + 1)];

}
