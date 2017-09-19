#include "common.h"
#include "vga.h"
#include "interrupt_handler.h"
#include "timer.h"
#include "paging.h"

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
    u64 base_addr, length;
    u32 type;
} __attribute__((__packed__));

extern "C" u32 multiboot, multiboot_magic;

extern "C" void kmain()
{
	vga.Clear();
	vga.Print("loxos\n");
    vga.Print("mbt: 0x%? 0x%?\n", multiboot_magic, multiboot);
    MultibootInfo* bootInfo = (MultibootInfo*)(u64)multiboot;
    vga.Print("flags: %?\n", (u64)bootInfo->flags);
    vga.Print("mem: %?KB -> %?KB\n", (int)bootInfo->mem_lower, (int)bootInfo->mem_upper);
    vga.Print("mmap: 0x%? (0x%?)\n", bootInfo->mmap_addr, bootInfo->mmap_length);

    for (MemoryMap* mmap = (MemoryMap*)(u64)bootInfo->mmap_addr; 
        (u64)mmap < bootInfo->mmap_addr + bootInfo->mmap_length;
        mmap = (MemoryMap*)((u64)mmap + mmap->size + sizeof(mmap->size)))
    {
        vga.Print("  size = 0x%? base_addr = 0x%? len = 0x%? type = 0x%?\n",
            mmap->size, mmap->base_addr, mmap->length, mmap->type);
    }

	interrupts.Setup();
	timer.Init(50);
    paging.Init();

    vga.Print("Paged!\n");
	while (true) {}
}
