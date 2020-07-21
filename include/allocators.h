#ifndef __ALLOCATORS_H__
#define __ALLOCATORS_H__

#include <stdio.h>

/// Use in the same function and deallocate at the same time
class LinearAllocator
{
    size_t offset;
    char* memory;
public:
    LinearAllocator()
    {
        memory = malloc(1<<24);
        offset = 0;
    }
    ~LinearAllocator()
    {
        free(memory);
    }

    char* allocate(size_t bytes)
    {
        char* ptr = memory + offset;
        offset+=bytes;
        return ptr;
    }

    void deallocate(size_t bytes)
    {
        offset -= bytes;
    }

};

#endif