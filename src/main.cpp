#include "common.h"
#include "vga.h"
#include "paging.h"
#include "gdt.h"
#include "isr.h"
#include "timer.h"
#include "keyboard.h"
#include "kalloc.h"
#include "fs.h"
#include "lox/lox.h"

struct ModEntry
{
    u32 start_addr;
    u32 end_addr;
} __attribute__((__packed__));

struct MultibootInfo
{
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    ModEntry* mods_addr;
    u32 elf_num, elf_size, elf_addr, elf_shndx;
    u32 mmap_length;
    u32 mmap_addr;
} __attribute__((__packed__));

bool g_completedLine = false;
char g_currentLine[128];
int g_currentLineIdx = 0;

static void onkey(const KeyInfo& keyInfo)
{
    if (keyInfo.keyDown)
    {
        if (keyInfo.charValue >= ' ' && keyInfo.charValue <= '~')
        {
            g_currentLine[g_currentLineIdx++] = keyInfo.charValue;
            vga.Puts(keyInfo.charValue);
        }
        else if (keyInfo.key == Key::Backspace && g_currentLineIdx > 0)
        {
            --g_currentLineIdx;
            vga.Backspace();
        }
        else if (keyInfo.key == Key::Enter || g_currentLineIdx >= 126)
        {
            g_currentLine[g_currentLineIdx] = '\n';
            g_currentLine[g_currentLineIdx+1] = '\0';
            g_completedLine = true;
        }
    }
}

extern "C" void kmain(MultibootInfo* bootInfo)
{
    vga.Clear();
    vga.Print("loxos\n");
    vga.Print("flags: %?\n", bootInfo->flags);
    vga.Print("mem: %?MB available\n", (int)(bootInfo->mem_upper - bootInfo->mem_lower) / 1024);

    kassert(bootInfo->mods_count > 0);
    u32 initrd_loc = bootInfo->mods_addr->start_addr;
    u32 initrd_end = bootInfo->mods_addr->end_addr;
    vga.Print("start: %? end: %?\n", initrd_loc, initrd_end);

    init_gdt();
    init_idt();
    init_timer();
    init_paging(initrd_end);
    kalloc_init(0xC000000, 0x1000000);
    init_keyboard();
    register_keyboard_handler(onkey);
    fs_init(initrd_loc, initrd_end);

    asm ("sti");
    vga.Print("OK\n");

    while (true) 
    {
        vga.Print("> ");
        while (!g_completedLine) 
        {}
        vga.Puts('\n');
        lox_run(g_currentLine, g_currentLineIdx);
        g_completedLine = false;
        g_currentLineIdx = 0;
    }

    //asm volatile("sti");
}
