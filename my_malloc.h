//
// Homework1-Dynamic Memory Allocation
// Name:    Ruitong Su
// NetID:   rs590
//

#ifndef HOMEWORK_1_DYNAMIC_MEMORY_ALLOCATION_MY_MALLOC_H
#define HOMEWORK_1_DYNAMIC_MEMORY_ALLOCATION_MY_MALLOC_H

#include <stdio.h>
#include <stdlib.h>
// sbrk()
#include <unistd.h>

#define HEADER_SIZE sizeof(blk_header)

// Header for each malloced block
struct block_header {
    size_t sz;
    int isFree;
    struct block_header *next;
    struct block_header *prev;
};
typedef struct block_header blk_header;

/*****Global Variables*****/
// maintain the free list
blk_header *fl_head = NULL;
blk_header *fl_tail = NULL;

/*****Utility Functions*****/
// call sbrk() for more space
void *request_new_block(size_t size);

// return sbrk(0)
void *get_heap_start_addr();

// update block header
void update_header(blk_header *blk, size_t sz, int isFree,
                   blk_header *next, blk_header *prev);

// First Fir strategy
// find the first suitable block in the free list
// not found -> return NULL
void *find_first_block(size_t size);

// allocate the block for the malloc request
void *allocate_block(blk_header *free_blk, size_t size);

// remove block from free list
void remove_block(blk_header *blk);

// split the block
void split_block(blk_header *blk, size_t payload);

// add block into free list
void add_block(blk_header *blk);

// Best Fit strategy
// Find the best suitable block in the free list
void *find_best_block(size_t size);

// traverse through the free list to find the largest block
unsigned long get_largest_free_data_segment_size();//in bytes

// traverse through the free list to sum up the size of
// all the free blocks
unsigned long get_total_free_size();//in bytes

/*****First Fit*****/
// malloc
void *ff_malloc(size_t size);

// free
void ff_free(void *ptr);

/*****Best Fit*****/
// malloc
void *bf_malloc(size_t size);

// free
void bf_free(void *ptr);

#endif //HOMEWORK_1_DYNAMIC_MEMORY_ALLOCATION_MY_MALLOC_H
