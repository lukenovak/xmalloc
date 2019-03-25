#include "bktarena.h"
#include "bktnode.h"
#include "xmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>

void make_arenas (void) __attribute__ ((constructor));

__thread int preferred_arena = 0;
bktarena* arenas;
int numarenas;

void make_arenas (void) {
    numarenas = get_nprocs_conf() * 4;
    arenas = mmap(NULL, numarenas*sizeof(bktarena), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    for (int i = 0; i < numarenas; i++) {
        make_bktalloc(&arenas[i]);
    }
}

void takelock() {
    if (pthread_mutex_trylock(arenas[preferred_arena]->mutex) == EBUSY) {
        preferred_arena = (preferred_arena + 1) % numarenas;
        pthread_mutex_lock(arenas[preferred_arena]->mutex);
    }
}

void* xmalloc(size_t bytes) {
    if (bytes > 1024) {
        size_t* mem = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
        *mem = bytes + sizeof(size_t);
        return mem + 1;
    }
    else {
        takelock();
        void* mem = bktmalloc(bytes, arenas[preferred_arena]);
        pthread_mutex_unlock(arenas[preferred_arena]->mutex);
        return mem;
    }
}

void xfree(void* ptr) {
    size_t* size =  ptr & (~4095);
    if (*size > 1024) {
        bktnode* node = (bktnode*) size;
        pthread_mutex_lock(arenas[node->arena]);
        bktfree(node, node->arena);
        pthread_mutex_unlock(arenas[node->arena]);
        return;
    }
    else {
        munmap(size, *size);
        return;
    }
}

void* xrealloc(void* prev, size_t bytes) {
    bktnode* node = ptr & (~4095);
    pthread_mutex_lock(arenas[node->arena]);
    void* newmem = bktmalloc(bytes, arenas[node->arena]);
    memcpy(newmem, prev, node->size);
    bktfree(node, prev);
    pthread_mutex_unlock(arenas[node->arena]);
    return newmem;
}
