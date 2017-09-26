#include "paging.h"
#include "vga.h"
#include "common.h"

PageDirectory pageDir;

void setup_paging()
{
    zero_memory(&pageDir, sizeof(PageDirectory));
    align_allocator();
    PageTable* table = new PageTable();
    zero_memory(table, sizeof(PageTable));
    pageDir.tables[0] = (u32)table | 0x7;
    vga.Print("0x%? 0x%?\n", pageDir.tables[0], pageDir.tables[1]);
    for (int j = 0; j<1024; j++)
    {
        table->pages[j].frame = j;
        table->pages[j].present = true;
        table->pages[j].rw = true;
        //vga.Print("0x%?\n", table->pages[j]);
    }

    vga.Print("Id mapped?\n");
    asm volatile("mov %0, %%cr3":: "r"(&pageDir));
    u32 cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));

    int* ptr = 0x0;
    *ptr = 0x1;

    vga.Print("BOOM\n");
}
