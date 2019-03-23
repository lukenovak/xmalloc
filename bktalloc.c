//TODO: map a page and make the requisite number of arenas
//for now, I'm assuming they are in a global bktarena* arenas
__thread int preferred_arena;

void takelock() {
    if (pthread_mutex_trylock(arenas[preferred_arena]->mutex) == EBUSY) {
        preferred_arena ++;
        pthread_mutex_lock(arenas[preferred_arena]->mutex);
    }
}

void* xmalloc(size_t bytes) {
    takelock();
    void* mem = bktmalloc(bytes, arenas[preferred_arena]);
    pthread_mutex_unlock(arenas[preferred_arena]->mutex);
    return mem;
}

void xfree(void* ptr) {
    bktnode* node = ptr & (~4095);
    pthread_mutex_lock(arenas[node->arena]);
    bktfree(node, arena);
    pthread_mutex_unlock(arenas[node->arena]);
    return;
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
