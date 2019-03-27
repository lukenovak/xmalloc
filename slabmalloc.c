// CH02 Slab Page allocator
// mmaps a huge page and breaks it into normal sized pages
#include "slabmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

// to keep track of the free pages
void* pages = NULL;
int pages_alloced = 0;
size_t PAGE_SIZE = 4096;

void*
slabmalloc(int numpages) {
    int page = __sync_fetch_and_add(&pages_alloced, numpages);
    if (pages && page < 1000) {    
        void* mem = pages + (PAGE_SIZE * page);
        return mem;

    }
    pages = mmap(0, 1000 * PAGE_SIZE, PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE, 1, 0);
    pages_alloced = numpages;
    return pages;
}

