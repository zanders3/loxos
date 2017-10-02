#pragma once

#include "common.h"

namespace PageFlags
{
    static const u32 Present = 0x1;
    static const u32 RW = 0x2;
    static const u32 User = 0x4;
}

struct Page
{
    inline void Set(u32 frame, u32 flags)
    {
        value = frame | flags;
    }

    u32 value;
} __attribute__((packed));

struct PageTable
{
    Page pages[1024];
} __attribute__((packed));

static_assert(sizeof(PageTable) == 0x1000, "invalid size");

void init_paging();
void map_page(u32 virtualAddr, u32 flags);
