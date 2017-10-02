#include "common.h"
#include "vga.h"
#include "paging.h"
#include "gdt.h"
#include "isr.h"
#include "timer.h"

struct MultibootInfo
{
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 elf_num, elf_size, elf_addr, elf_shndx;
    u32 mmap_length;
    u32 mmap_addr;
} __attribute__((__packed__));

struct MemoryMap
{
    u32 size;
    u32 base_addr, base_addr_high, length, length_high;
    u32 type;
} __attribute__((__packed__));

extern "C" u32 code_end;

extern "C" void kmain(MultibootInfo* bootInfo, u32 multiboot_magic)
{
	vga.Clear();
	vga.Print("loxos\n");
    vga.Print("mbt: 0x%?\n", multiboot_magic);
    vga.Print("flags: %?\n", bootInfo->flags);
    vga.Print("mem: %?KB -> %?KB\n", (int)bootInfo->mem_lower, (int)bootInfo->mem_upper);
    vga.Print("mmap: 0x%? (0x%?)\n", bootInfo->mmap_addr, bootInfo->mmap_length);

    for (MemoryMap* mmap = (MemoryMap*)(u32)bootInfo->mmap_addr; 
        (u32)mmap < bootInfo->mmap_addr + bootInfo->mmap_length;
        mmap = (MemoryMap*)((u32)mmap + mmap->size + sizeof(mmap->size)))
    {
        vga.Print("  base_addr = 0x%? len = 0x%? type = %?\n",
            (u32)mmap->base_addr, (u32)mmap->length, (int)mmap->type);
    }

    vga.Print("0x%?\n", (u32)&code_end);
    init_gdt();
    init_idt();
    init_timer();
    init_paging();

    map_page(0xC000000, PageFlags::RW);
    int* test = (int*)0xC000000;
    *test = 0xdead;
    vga.Print("test val: %?\n", (u32)*test);

    while (true) {}

    //asm volatile("sti");
}
