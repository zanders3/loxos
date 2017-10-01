#include "paging.h"
#include "vga.h"
#include "isr.h"

extern "C" u32 code_end;
u32 g_placement_address;

static PageTable* alloc_page(u32& physAddr)
{
    if (g_placement_address & 0xFFFFF000)
        g_placement_address = (g_placement_address & 0xFFFFF000) + 0x1000;
    physAddr = g_placement_address;
    g_placement_address += sizeof(PageTable);
    return (PageTable*)physAddr;
}

void init_paging()
{
    g_placement_address = (u32)&code_end;

    //Create page directory, map to itself
    u32 pageDirAddr;
    PageTable* dir = alloc_page(pageDirAddr);
    zero_memory(dir, sizeof(PageTable));
    dir->pages[1023].Set(pageDirAddr, PageFlags::RW | PageFlags::Present);

    //Identity map first 0x40 0000
    kassert(g_placement_address < 0x1000*1024);
    u32 pageAddr;
    PageTable* page = alloc_page(pageAddr);
    dir->pages[0].Set(pageAddr, PageFlags::RW | PageFlags::Present);
    for (u32 i = 0; i<1024; i++)
        page->pages[i].Set(i*0x1000, PageFlags::RW | PageFlags::Present);

    //Activate paging
    asm volatile("mov %0, %%cr3"::"r"(pageDirAddr));
    u32 cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0"::"r"(cr0));

    //Check some stuff
    vga.Print("Dir 0 val: %? (%?)\n", dir->pages[0].value, pageDirAddr);
    vga.Print("Page 0 val: %? (%?)\n", page->pages[20].value, pageAddr);
    dir = (PageTable*)0xFFFFF000;
    page= (PageTable*)0xFFC00000;
    vga.Print("Dir 0 val: %? (%?)\n", dir->pages[0].value, pageDirAddr);
    vga.Print("Page 0 val: %?\n", page->pages[20].value);
    vga.Print("OK!\n");

    //ID Map kernel
    /*while (int i = 0x0; i < code_end; i+=0x1000)
        map_page(i, i, PageFlags::RW | PageFlags::Present);*/
}

void map_page(u32 virtualAddr, u32 flags)
{
    u32 dirIdx = virtualAddr >> 22;//top 10 bits
    u32 pageIdx = virtualAddr >> 12 & 0x3FF;//next 10 bits from top

    //Create the page table it doesn't exist yet
    PageTable* dir = (PageTable*)0xFFFFF000;
    PageTable* page = ((PageTable*)0xFFC00000) + dirIdx;
    if (dir->pages[pageIdx].value == 0x0)
    {
        u32 pageAddr;
        alloc_page(pageAddr);
        dir->pages[pageIdx].Set(pageAddr, PageFlags::RW | PageFlags::Present);
        zero_memory(page, sizeof(PageTable));
    }

    //Page table exists at this point!
    kassert(page->pages[pageIdx].value == 0x0);//should not already be mapped

    u32 memAddr;
    alloc_page(memAddr);
    page->pages[pageIdx].Set(memAddr, flags | PageFlags::Present);
}
