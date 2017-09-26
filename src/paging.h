#pragma once

#include "common.h"

struct Page
{
	bool present   : 1;   //Page present in memory
	bool rw        : 1;
	bool user      : 1;
	bool accessed  : 1;
	bool dirty     : 1;
	u32 unused     : 7;
	u32 frame      : 20;	
} __attribute__((__packed__));

static_assert(sizeof(Page) == sizeof(u32), "Page size");

struct PageTable
{
    Page pages[1024];
} __attribute__((__packed__));

static_assert(sizeof(PageTable) == sizeof(u32)*1024, "Page size");

struct PageDirectory
{
    u32 tables[1024];
} __attribute__((__packed__));
static_assert(sizeof(PageDirectory) == sizeof(u32)*1024, "Page size");

void setup_paging();
Page& get_page(u32 memoryLocation);
