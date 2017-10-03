#pragma once
#include "common.h"

void kalloc_init(u32 memStart, u32 memSize);
void* kalloc(u32 size, bool exactFit);
void kfree(void* ptr);

template <typename T> inline T* kalloc()
{
    return (T*)kalloc(sizeof(T), true);   
}

class KAllocator {};
extern KAllocator kallocator;
inline void* operator new(size_t size, KAllocator&)
{
    return kalloc(size, true);
}
inline void* operator new[](size_t size, KAllocator&)
{
    return kalloc(size, false);
}