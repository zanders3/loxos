#pragma once

#include "common.h"
#include "interrupt_handler.h"

u64 kmalloc(u64 sz, bool align = false);

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

struct PageTable
{
    Page pages[1024];
} __attribute__((__packed__));

struct PageDirectory
{
    //Array of pointers to page tables
    PageTable* tables[1024];
    //Array of points to page tables,
    //but physical location for loading into CR3 register
    u64 tablesPhysical[1024];
} __attribute__((__packed__));

class Paging
{
public:
    void Init();
    void AllocFrame(Page* page, bool isKernel, bool isWritable);
    Page* GetPage(u64 address, bool create, PageDirectory* directory);
    void SwitchPageDirectory(PageDirectory* directory);

private:
    //Page fault handler
    static void PageFault(const Registers& reg);
};
extern Paging paging;
