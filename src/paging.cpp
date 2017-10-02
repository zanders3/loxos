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

static void handle_page_fault(const Registers& regs)
{
    vga.Print("Unhandled interrupt:\nPage fault (%?) code %? at 0x%?\n",
        (int)regs.interrupt, 
        (int)regs.err_code,
        regs.eip);

    u32 memLoc;
    asm volatile("mov %%cr2,%0" : "=r"(memLoc));

    vga.Print("(");
    vga.Print(regs.err_code & 0x1 ? "protection-violation " : "not-present ");
    vga.Print(regs.err_code & 0x2 ? "write " : "read ");
    vga.Print(regs.err_code & 0x4 ? "user-mode" : "supervisor-mode");
    if (regs.err_code & 0x10) vga.Print(" inst-fetch");
    vga.Print(") at 0x%?\n", memLoc);

    kpanic("");
}

void init_paging()
{
    g_placement_address = (u32)&code_end;

    //Create page directory, map to itself
    u32 pageDirAddr;
    PageTable* dir = alloc_page(pageDirAddr);
    zero_memory(dir, sizeof(PageTable));
    dir->pages[1023].Set(pageDirAddr, PageFlags::RW | PageFlags::Present);

    //Identity map to g_placement_address
    u32 pageAddr;
    PageTable* page = alloc_page(pageAddr);
    kassert(g_placement_address < 0x1000*1024);//assumes kernel < 4MB total

    //Set first directory entry to page
    dir->pages[0].Set(pageAddr, PageFlags::RW | PageFlags::Present);
    u32 numToMap = g_placement_address / 0x1000;
    kassert(numToMap < 1024);
    for (u32 i = 0; i<numToMap; i++)
        page->pages[i].Set(i*0x1000, PageFlags::RW | PageFlags::Present);

    register_int_handler(14, &handle_page_fault);

    //Activate paging
    asm volatile("mov %0, %%cr3"::"r"(pageDirAddr));
    u32 cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0"::"r"(cr0));
}

void map_page(u32 virtualAddr, u32 flags)
{
    u32 dirIdx = virtualAddr >> 22;//top 10 bits
    u32 pageIdx = virtualAddr >> 12 & 0x3FF;//next 10 bits from top

    //Create the page table it doesn't exist yet
    PageTable* dir = (PageTable*)0xFFFFF000;
    PageTable* page = ((PageTable*)0xFFC00000) + dirIdx;
    if (dir->pages[dirIdx].value == 0x0)
    {
        u32 pageAddr;
        alloc_page(pageAddr);
        dir->pages[dirIdx].Set(pageAddr, PageFlags::RW | PageFlags::Present);
        zero_memory(page, sizeof(PageTable));
    }

    //Page table exists at this point!
    kassert(page->pages[pageIdx].value == 0x0);//should not already be mapped

    u32 memAddr;
    alloc_page(memAddr);
    page->pages[pageIdx].Set(memAddr, flags | PageFlags::Present);
}
