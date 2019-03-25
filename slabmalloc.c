// CH02 Slab Page allocator
// mmaps a huge page and breaks it into normal sized pages
#include "slabmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

// to keep track of the free pages
__thread void* pages = NULL;
__thread int pages_alloced = 0;
size_t PAGE_SIZE = 4096;

void*
slabmalloc() {
    if (pages && pages_alloced < 500) {
        ++pages_alloced;
        return &pages[pages_alloced];
    }
    pages = mmap(0, 500 * PAGE_SIZE, PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE, 1, 0);
    pages_alloced = 1;
    return pages;
}

