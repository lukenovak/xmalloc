#include "bktarena.h"
#include "bktnode.h"
#include "xmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

__thread bktarena arena = {{0,0,0,0,0,0,0,0}};

void* xmalloc(size_t bytes) {
    if (bytes > 2048) {
        size_t* mem = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
        *mem = bytes + sizeof(size_t);
        return mem + 1;
    }
    else {
        void* mem = bktmalloc(bytes, &arena);
        return mem;
    }
}

void xfree(void* ptr) {
    size_t* size = (size_t*) ((long)ptr & (~4095l));
    if (*size <= 2048) {
        bktnode* node = (bktnode*) size;
        bktfree(node, ptr);
        return;
    }
    else {
        munmap(size, *size);
        return;
    }
}

void* xrealloc(void* prev, size_t bytes) {
    void* newmem = xmalloc(bytes);
    size_t* prevsize = (size_t*) ((long)prev & (~4095l));
    memcpy(newmem, prev, *prevsize < bytes ? *prevsize : bytes);
    xfree(prev);
    return newmem;
}
