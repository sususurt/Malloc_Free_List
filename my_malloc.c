//
// Homework1-Dynamic Memory Allocation
// Name:    Ruitong Su
// NetID:   rs590
//

#include "my_malloc.h"

/*****First Fit*****/
void *ff_malloc(size_t size) {
    // Can not allocate 0 byte
    if (!size) {
        return NULL;
    }
    // find suitable block in free list
    blk_header *blk = find_first_block(size);
    if (blk) {
        // found
        // allocate block
        return allocate_block(blk, size);
    } else {
        // not found
        // request new block
        return request_new_block(size);
    }
}

void ff_free(void *ptr) {
    blk_header *cur_blk = (blk_header *) ((char *) ptr - HEADER_SIZE);
    cur_blk->isFree = 1;
    add_block(cur_blk);
    // Coalescing
    // [cur_blk][cur_blk_next]->[cur_blk+cur_blk_next]
    if (cur_blk->next != NULL) {
        char *cur_blk_next = (char *) cur_blk + cur_blk->sz + HEADER_SIZE;
        if ((char *) cur_blk->next == cur_blk_next) {
            cur_blk->sz += cur_blk->next->sz + HEADER_SIZE;
            remove_block(cur_blk->next);
        }
    }

    // [cur_blk_prev][cur_blk]->[cur_blk_prev+cur_blk]
    if (cur_blk->prev != NULL) {
        char *cur_blk_addr = (char *) cur_blk->prev + cur_blk->prev->sz + HEADER_SIZE;
        if ((char *) cur_blk == cur_blk_addr) {
            cur_blk->prev->sz += cur_blk->sz + HEADER_SIZE;
            remove_block(cur_blk);
        }
    }
}

/*****Best Fit*****/
void *bf_malloc(size_t size) {
    // Can not allocate 0 byte
    if (!size) {
        return NULL;
    }
    // find suitable block in free list
    blk_header *blk = find_best_block(size);
    if (blk) {
        // found
        // allocate block
        return allocate_block(blk, size);
    } else {
        // not found
        // request new block
        return request_new_block(size);
    }
}

void bf_free(void *ptr) {
    ff_free(ptr);
}

/*****Utility Functions*****/
void *request_new_block(size_t size) {
    // get the current heap start address
    blk_header *new_blk = get_heap_start_addr();
    // call sbrk() for more space
    // if sbrk() failed, return NULL
    if (sbrk(size + HEADER_SIZE) == (void *) -1) {
        return NULL;
    }
    // update the header for new block
    update_header(new_blk, size, 0, NULL, NULL);
    // convert to char* for align
    return (char *) new_blk + HEADER_SIZE;
}

void *get_heap_start_addr() {
    // sbrk(0) returns the address from
    // the top of the heap
    return sbrk(0);
}

void update_header(blk_header *blk, size_t _sz, int _isFree,
                   blk_header *_next, blk_header *_prev) {
    blk->sz = _sz;
    blk->isFree = _isFree;
    blk->next = _next;
    blk->prev = _prev;
}

void *find_first_block(size_t size) {
    blk_header *cur_blk = fl_head;
    while (cur_blk) {
        if (cur_blk->sz < size) {
            cur_blk = cur_blk->next;
        } else {
            return cur_blk;
        }
    }
    return NULL;
}

void *find_best_block(size_t size) {
    blk_header *cur_blk = fl_head, *best_blk = NULL;
    while (cur_blk) {
        if (cur_blk->sz < size) {
            cur_blk = cur_blk->next;
        } else if (cur_blk->sz > size) {
            if (best_blk == NULL || cur_blk->sz < best_blk->sz) {
                best_blk = cur_blk;
            }
            cur_blk = cur_blk->next;
        }
            // cur_blk->sz == size
        else {
            best_blk = cur_blk;
            break;
        }
    }
    return best_blk;
}

void *allocate_block(blk_header *free_blk, size_t size) {
    // if free_blk.size is larger than the payload plus a new header
    // which means the additional space can be split as a new free block
    if (free_blk->sz > size + HEADER_SIZE) {
        split_block(free_blk, size);
    } else {
        remove_block(free_blk);
    }
    update_header(free_blk, size, 0, NULL, NULL);
    return (char *) free_blk + HEADER_SIZE;
}

void remove_block(blk_header *blk) {
    // blk -> only one element
    if (fl_head == blk && fl_tail == blk) {
        fl_head = NULL;
        fl_tail = NULL;
    }
        // blk -> first element
    else if (fl_head == blk) {
        fl_head->next->prev = NULL;
        fl_head = blk->next;
        blk->next = NULL;
    }
        // blk -> last element
    else if (fl_tail == blk) {
        fl_tail->prev->next = NULL;
        fl_tail = blk->prev;
        blk->prev = NULL;
    }
        // blk -> middle
    else {
        blk->next->prev = blk->prev;
        blk->prev->next = blk->next;
    }
}

void split_block(blk_header *blk, size_t payload) {
    // split the blk
    blk_header *split_blk = (blk_header *) ((char *) blk + HEADER_SIZE + payload);
    // update the size of the new block
    size_t split_blk_sz = blk->sz - HEADER_SIZE - payload;
    // update other info of the new block
    update_header(split_blk, split_blk_sz, 1, NULL, NULL);
    // remove the blk
    remove_block(blk);
    // add the new block back to the free list
    add_block(split_blk);
}

void add_block(blk_header *blk) {
    // fl_head or free list is empty
    if (fl_head == NULL && fl_tail == NULL) {
        fl_head = blk;
        fl_tail = blk;
        update_header(blk, blk->sz, 1, NULL, NULL);
    }
        // blk.addr < fl_head
    else if (blk < fl_head) {
        update_header(blk, blk->sz, 1, fl_head, NULL);
        blk->next->prev = blk;
        fl_head = blk;
    }
        // fl_tail > blk.addr > fl_head
    else if (fl_tail > blk && blk > fl_head) {
        blk_header *cur_blk = fl_head;
        while (cur_blk != NULL) {
            if (blk > cur_blk) {
                cur_blk = cur_blk->next;
            } else {
                break;
            }
        }
        update_header(blk, blk->sz, 1, cur_blk, cur_blk->prev);
        cur_blk->prev = blk;
        blk->prev->next = blk;
    }
        // blk.addr > fl_tail
    else {
        update_header(blk, blk->sz, 1, NULL, fl_tail);
        blk->prev->next = blk;
        fl_tail = blk;
    }
}

unsigned long get_largest_free_data_segment_size() {
    blk_header *cur_blk = fl_head;
    unsigned long largest_size = 0;
    while (cur_blk) {
        if (cur_blk->sz > largest_size) {
            largest_size = cur_blk->sz;
        }
        cur_blk = cur_blk->next;
    }
    return largest_size;
}

unsigned long get_total_free_size() {
    blk_header *cur_blk = fl_head;
    unsigned long total_size = 0;
    while (cur_blk) {
        total_size += cur_blk->sz;
        cur_blk = cur_blk->next;
    }
    return total_size;
}