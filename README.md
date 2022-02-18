# Malloc_Free_list

> Duke-sp22-ECE650-HW1

# Overview

This project implements `malloc()` and `free()`, with **Best-Fit** and **First-Fit** algorithm and **Free List**
strategy .

# Data Structure

Maintain a doubly linked list, which contains all the free block in the heap.

Each block contains a header, record the meta info of the block: size of the payload, is free or not, pointer to the
next free block and pointer to the previous free block.

When updating the free list, the new free blocks are inserted into the free list in address order each time.