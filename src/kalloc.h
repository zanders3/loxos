#pragma once
#include "common.h"

void kalloc_init(u32 memStart, u32 memSize);
void* kalloc(u32 size, bool exactFit);
void kfree(void* ptr);
int kalloc_count();

template <typename T> inline T* kalloc()
{
    return (T*)kalloc(sizeof(T), true);   
}

template <typename T> inline T* kallocArr(u32 count)
{
    return (T*)kalloc(sizeof(T) * count, false);
}

class KAllocator {};
extern KAllocator kallocator;
inline void* operator new(size_t size, KAllocator&)
{
    return kalloc(size, true);
}