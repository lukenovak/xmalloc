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

void make_arenas (void) __attribute__ ((constructor));

__thread int preferred_arena = 0;
bktarena* arenas;
int numarenas;

void make_arenas (void) {
    numarenas = get_nprocs_conf() * 4;
    arenas = mmap(NULL, numarenas*sizeof(bktarena), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    for (int i = 0; i < numarenas; i++) {
        make_bktarena(&arenas[i], i);
    }
}

static void takelock() {
    while (pthread_mutex_trylock(&arenas[preferred_arena].mutex) == EBUSY) {
        preferred_arena = (preferred_arena + 1) % numarenas;
    }
}

void* xmalloc(size_t bytes) {
    if (bytes > 2048) {
        size_t* mem = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
        *mem = bytes + sizeof(size_t);
        return mem + 1;
    }
    else {
        takelock();
        void* mem = bktmalloc(bytes, &arenas[preferred_arena]);
        pthread_mutex_unlock(&arenas[preferred_arena].mutex);
        return mem;
    }
}

void xfree(void* ptr) {
    size_t* size = (size_t*) ((long)ptr & (~4095l));
    if (*size <= 2048) {
        bktnode* node = (bktnode*) size;
        pthread_mutex_lock(&arenas[node->arena].mutex);
        bktfree(node, ptr);
        pthread_mutex_unlock(&arenas[node->arena].mutex);
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
