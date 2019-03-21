// HW08 Simple Allocator
// By Elijah Steres, modified for thread safety by Luke Novak
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "hmalloc.h"

/*
  typedef struct hm_stats {
  long pages_mapped;
  long pages_unmapped;
  long chunks_allocated;
  long chunks_freed;
  long free_length;
  } hm_stats;
*/

typedef struct free_block {
        size_t size;
            struct free_block* next;
} free_block;

const size_t PAGE_SIZE = 4096;
static hm_stats stats; // This initializes the stats to 0.
static free_block* free_list = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

long
free_list_length()
{
    free_block* element = free_list;
    int length = 0;
    while (element != NULL) {
        length ++;
        element = element->next;
    }
    return length;
}

hm_stats*
hgetstats()
{
    stats.free_length = free_list_length();
    return &stats;
}

void
hprintstats()
{
    stats.free_length = free_list_length();
    fprintf(stderr, "\n== husky malloc stats ==\n");
    fprintf(stderr, "Mapped:   %ld\n", stats.pages_mapped);
    fprintf(stderr, "Unmapped: %ld\n", stats.pages_unmapped);
    fprintf(stderr, "Allocs:   %ld\n", stats.chunks_allocated);
    fprintf(stderr, "Frees:    %ld\n", stats.chunks_freed);
    fprintf(stderr, "Freelen:  %ld\n", stats.free_length);
}

static
size_t
div_up(size_t xx, size_t yy)
{
    // This is useful to calculate # of pages
    // for large allocations.
    size_t zz = xx / yy;

    if (zz * yy == xx) {
        return zz;
    }
    else {
        return zz + 1;
    }
}

void coalesce(free_block* item, free_block* next) {
    if(((void*)item) + item->size == next) { //next lands right after item, coalesce
        item->size = item->size + next->size;
        item->next = next->next;
    }
}

void* div_block(void* block, size_t block_size, size_t div_size) {
    if (block_size - div_size > sizeof(free_block)) {
        *(size_t*)block = div_size;

        free_block* new_block = (free_block*) (block + div_size);
        new_block->size = block_size - div_size;

        free_block** item = &free_list;
        while (*item != NULL) {
            if (*item > new_block) {
                new_block->next = *item;
                *item = new_block;
                coalesce(free_list, free_list->next);
                return block+sizeof(size_t);
            }
            item = &(*item)->next;
        }
        *item = new_block;
        new_block->next = NULL;
    }
    return block + sizeof(size_t);
}

void*
hmalloc(size_t size)
{
    stats.chunks_allocated += 1;
    size += sizeof(size_t);
    if (size < sizeof(free_block)) {
        size = sizeof(free_block);
    }

    if (size <= 4096) {
        pthread_mutex_lock(&mutex);
        free_block** item = &free_list;
        while (*item != NULL) {
            if ((*item)->size >= size) {
                free_block* block = *item;
                *item = (*item)->next; // fix references
                void* new_block = div_block(block, block->size, size);
                pthread_mutex_unlock(&mutex);
                return new_block;
            }
            item = &(*item)->next;
        }
        void* new_page = mmap(((void*)free_list)-4096, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
        stats.pages_mapped ++;

        *(size_t*)new_page = 4096;
        new_page = div_block(new_page, 4096, size);
        pthread_mutex_unlock(&mutex);
        return new_page;
    }
    else {
        long pages = div_up(size, 4096);
        size = 4096*pages;
        void* new_block = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
        stats.pages_mapped += pages;
        *(size_t*)new_block = size;
        new_block += sizeof(size_t);
        return new_block;
    }
}

void
hfree(void* item)
{
    stats.chunks_freed += 1;
    item -= sizeof(size_t);
    free_block* block = (free_block*) item;
    if (block->size > 4096) {
        stats.pages_unmapped += block->size/4096;
        munmap(block, block->size);
    }
    else {
        pthread_mutex_lock(&mutex);
        if (free_list == NULL) {
            free_list = block;
            block->next = NULL;
            pthread_mutex_unlock(&mutex);
            return;
        }
        free_block* prev = NULL;
        free_block* elem = free_list;
        while(elem != NULL) {
            if(elem > block) {
                block->next = elem;
                coalesce(block, elem);
                if (prev != NULL) {
                    prev->next = block;
                    coalesce(prev, block);
                }
                else {
                    free_list = block;
                }
                pthread_mutex_unlock(&mutex);
                return;
            }
            prev = elem;
            elem = elem->next;
        }
        prev->next = block;
        block->next = NULL;
        coalesce(prev, block);
        pthread_mutex_unlock(&mutex);
        return;
    }
}

void*
realloc(void* item, size_t bytes)
{
    void* new_alloc = malloc(bytes);
    memcpy(item, new_alloc, bytes);
    free(item);

}
