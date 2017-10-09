#pragma once
#include "common.h"

void kalloc_init(u32 memStart, u32 memSize);
void* kalloc(u32 size, bool exactFit);
void kfree(void* ptr);
int kalloc_count();

template <typename T> inline T* kalloc()
{
    void* ptr = kalloc(sizeof(T), true);   
    return new (ptr) T;
}

template <typename T> inline T* kallocArr(u32 count)
{
    return (T*)kalloc(sizeof(T) * count, false);
}
