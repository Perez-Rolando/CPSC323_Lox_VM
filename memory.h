//The Memory Management Utilities
//The macros used for dynamic array growth and the memory allocation
#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

//Allocating Memory for count items of type
#define ALLOCATE(type, count) \
    (type*)reallocate(nullptr, 0, sizeof(type) * (count))

//FREE a single Item
#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

//GROW_CAPACITY calculates the new capacity for a growing array 
// THE STRAT USED: Start @8 and double each time from there
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

//GROW_ARRAY Reallocates an array to a new size
//A wrapper around that handles the type casting
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

//FREE_ARRAY put simply it frees a dynamic array to be reused
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

/**
* reallocate = This is the core memory allocation function it consist of:
* -pointer The existing memory block (or nullptr for new allocation)
* -oldSize The current size in bytes
* -newSize The desired size in bytes
* -Pointer to the resized memory block
*/
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif