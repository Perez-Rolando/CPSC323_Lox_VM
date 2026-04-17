//The Memory Management Implementation
#include <cstdlib>
#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    // If newSize is 0, we're freeing memory
    if (newSize == 0) {
        free(pointer);
        return nullptr;
    }

    // use realloc which is able to handle 
    // - Allocation if the pointer is nullptr
    // - Resizing if the pointer is not nullptr
    void* result = realloc(pointer, newSize);

    // returns nullptr if the allocation ends up failing 
    if (result == nullptr) {
        exit(1);
    }
    
    return result;
}