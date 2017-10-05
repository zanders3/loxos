#include "common.h"
#include "vga.h"
#include "paging.h"
#include "gdt.h"
#include "isr.h"
#include "timer.h"
#include "keyboard.h"
#include "kalloc.h"
#include "karray.h"

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

static void onkey(const KeyInfo& keyInfo)
{
    //vga.Print("K: %? %? %? %?\n", (int)keyInfo.key, keyInfo.charValue, (int)keyInfo.keyDown, (u32)keyInfo.scanCode);
    if (keyInfo.keyDown && 
        ((keyInfo.charValue >= ' ' && keyInfo.charValue <= '~') || keyInfo.charValue == '\n'))
        vga.Puts(keyInfo.charValue);
}

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
    kalloc_init(0xC000000, 0x1000000);
    init_keyboard();
    register_keyboard_handler(onkey);

    asm ("sti");
    vga.Print("OK\n");
    
    Array<int> test;
    for (int i = 0; i<20; i++)
        test.Add(i);
    test.Insert(5, 23);
    test.Insert(2, 85);
    test.Sort();
    test.RemoveAt(0, 10);

    for (const int& val : test)
        vga.Print("%? ", val);

    vga.Print("\n");

    vga.Print("OK\n");
    while (true) {}

    //asm volatile("sti");
}
