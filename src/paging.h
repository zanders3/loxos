#pragma once

#include "common.h"

struct Page
{
    u32 present    : 1;   // Page present in memory
    u32 rw         : 1;   // Read-only if clear, readwrite if set
    u32 user       : 1;   // Supervisor level only if clear
    u32 accessed   : 1;   // Has the page been accessed since last refresh?
    u32 dirty      : 1;   // Has the page been written to since last refresh?
    u32 unused     : 7;   // Amalgamation of unused and reserved bits
    u32 frame      : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

struct PageTable
{
    Page pages[1024];
} __attribute__((packed));

static_assert(sizeof(PageTable) == 0x1000, "invalid size");

struct PageDirectory
{
    PageTable* tables[1024];
    u32 tablesPhysical[1024];
};

void init_paging();
