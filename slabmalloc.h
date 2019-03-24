#ifndef SLABMALLOC_H
#define SLABMALLOC_H

#include <stddef.h>

// mmaps a HUGE_PAGE and breaks it into a stack of normal pages
void* slabmalloc();

#endif
