// CH02 Slab Page allocator
// mmaps a huge page and breaks it into normal sized pages
#include "slabmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

// to keep track of the free pages
__thread void* pages = NULL;
__thread int pages_alloced = 0;

void*
slabmalloc() {
    if (pages && pages_alloced < 500) {
        ++pages_alloced;
        return &pages[pages_alloced];
    }
    pages = mmap(0, 4096, O_READ | O_WRITE, 
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB | MAP_HUGE_2MB, 1, 0);
    pages_alloced = 1;
    return pages;
}

