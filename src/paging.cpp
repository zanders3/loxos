#include "paging.h"
#include "vga.h"
#include "isr.h"

u32 placement_address;
u32 first_free_frame;
u32 total_frames;

extern "C" u32 code_end;

static u32 kmalloc(u32 sz, bool align, u32* phys)
{
    if (align && (placement_address & 0xFFFFF000))
    {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys)
        *phys = placement_address;
    
    u32 tmp = placement_address;
    placement_address += sz;
    return tmp;
}

static u32 kmalloca(u32 sz)
{
    return kmalloc(sz, true, nullptr);
}

static u32 kmallocap(u32 sz, u32* phys)
{
    return kmalloc(sz, true, phys);
}

static Page* get_page(u32 address, bool create, PageDirectory* dir)
{
    address /= 0x1000;
    u32 table_idx = address / 1024;
    if (dir->tables[table_idx])
        return &dir->tables[table_idx]->pages[address%1024];
    else if (create)
    {
        u32 tmp;
        dir->tables[table_idx] = (PageTable*)kmallocap(sizeof(PageTable), &tmp);
        zero_memory(dir->tables[table_idx], sizeof(PageTable));
        dir->tablesPhysical[table_idx] = tmp | 0x7;//PRESENT, RW, US
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
        return nullptr;
}

static void alloc_frame(Page* page, bool isKernel, bool isWriteable)
{
    if (page->frame)
        return;
    if (first_free_frame >= total_frames)
        kpanic("No free frames!");

    page->frame = first_free_frame;
    first_free_frame++;
    page->present = 1;
    page->rw = isWriteable ? 1 : 0;
    page->user = isKernel ? 0 : 1;
}

static void switch_page_directory(PageDirectory* dir)
{
    asm volatile("mov %0, %%cr3"::"r"(&dir->tablesPhysical));
    u32 cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0"::"r"(cr0));
}

static void page_fault(const Registers& regs)
{
    vga.Print("Unhandled interrupt:\nPage fault (%?) at 0x%?\n",
        (int)regs.interrupt, 
        regs.eip);

    u32 faulting_addr;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_addr));

    vga.Print("(");
    if (!(regs.err_code & 0x1)) { vga.Print("not-present "); }
    if (regs.err_code & 0x2) { vga.Print("write-op "); }
    if (regs.err_code & 0x4) { vga.Print("user-mode "); }
    if (regs.err_code & 0x8) { vga.Print("overwritten-reserved "); }
    if (regs.err_code & 0x10) { vga.Print("instruction-fetch "); }
    vga.Print(") at 0x%?\n", faulting_addr);
    kpanic("");
}

void init_paging()
{
    const u32 mem_end_page = 0x1000000;//assume 16MB
    total_frames = mem_end_page / 0x1000;
    first_free_frame = 0;
    placement_address = (u32)&code_end;

    PageDirectory* kernelDirectory = (PageDirectory*)kmalloca(sizeof(PageDirectory));
    zero_memory(kernelDirectory, sizeof(PageDirectory));

    u32 i = 0x0;
    while (i < placement_address)
    {
        Page* page = get_page(i, true, kernelDirectory);
        if (!page)
            kpanic("page didn't alloc");
        alloc_frame(page, false, false);
        i += 0x1000;
    }

    vga.Print("alloc %?/%? frames (0x%?)\n", (int)first_free_frame, (int)total_frames, placement_address);

    register_int_handler(14, &page_fault);

    switch_page_directory(kernelDirectory);
    vga.Print("Hello paging!\n");

    u32* ptr = (u32*)0xA0000000;
    *ptr = 0;
}
