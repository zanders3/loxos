#include "paging.h"
#include "interrupt_handler.h"
#include "vga.h"

extern "C" u64 code_end;
u64 malloc_address;
Paging paging;

class FrameBitset
{
public:
    static inline u32 IndexFromBit(u32 a) { return a/(8*4); }
    static inline u32 OffsetFromBit(u32 a) { return a%(8*4); }

    void Init(u64 memSize)
    {
        numFrames = memSize / 0x1000;
        int numFrameInts = IndexFromBit(numFrames);
        vga.Print("Num pages: %? Mem: 0x%?\n", (int)numFrames, malloc_address);

        frames = (u32*)kmalloc(IndexFromBit(numFrames));
        zero_memory(frames, IndexFromBit(numFrames));
    }
    void SetFrame(u64 frameAddr)
    {
        u32 frame = frameAddr / 0x1000;
        u32 idx = IndexFromBit(frame);
        u32 off = OffsetFromBit(frame);
        frames[idx] |= (1 << off);
    }
    void ClearFrame(u64 frameAddr)
    {
        u32 frame = frameAddr / 0x1000;
        u32 idx = IndexFromBit(frame);
        u32 off = OffsetFromBit(frame);
        frames[idx] &= ~(1 << off);
    }
    bool TestFrame(u64 frameAddr)
    {
        u32 frame = frameAddr / 0x1000;
        u32 idx = IndexFromBit(frame);
        u32 off = OffsetFromBit(frame);
        return frames[idx] & (1 << off);
    }
    u32 FirstFreeFrame()
    {
        for (u32 i = 0; i<IndexFromBit(numFrames); ++i)
        {
            if (frames[i] != 0xFFFFFFFF)
            {
                for (u32 j = 0; j<32; ++j)
                {
                    u32 toTest = 1 << j;
                    if (!(frames[i] & (1 << j)))
                        return (i*4*8)+j;
                }
            }
        }
        return -1;
    }
private:
    u32* frames;
    u32 numFrames;
};
FrameBitset frameBitset;

u64 kmalloc(u64 sz, bool align)
{
    if (align && (malloc_address & 0xFFFFF000))
    {
        malloc_address &= 0xFFFFF000;
        malloc_address += 0x1000;
    }
    u64 tmp = malloc_address;
    malloc_address += sz;
    return tmp;
}

void Paging::Init()
{
    interrupts.RegisterHandler(14, &Paging::PageFault);
    u64 codeEndVal = (u64)&code_end;
    malloc_address = codeEndVal;

    const u32 mem_end_page = 0x1000000;//32 MB max memory size
    frameBitset.Init(mem_end_page);

    vga.Print("Loc: %? > %?\n", sizeof(PageDirectory), malloc_address);
    PageDirectory* pageDir = (PageDirectory*)kmalloc(sizeof(PageDirectory), true);
    vga.Print("Loc: %?\n", (u64)pageDir);
    zero_memory(pageDir, sizeof(PageDirectory));
    
    for (u64 i = 0; i<malloc_address; i+=0x1000)
    {
        AllocFrame(GetPage(i, true, pageDir), false, false);
    }

    SwitchPageDirectory(pageDir);
}

void Paging::AllocFrame(Page* page, bool isKernel, bool isWritable)
{
    if (page->frame != 0)
        return;

    u32 idx = frameBitset.FirstFreeFrame();
    if (idx == (u32)-1)
        kpanic("No free frames!");

    frameBitset.SetFrame(idx * 0x1000);
    page->present = true;
    page->rw = isWritable;
    page->user = !isKernel;
    page->frame = idx;
}

Page* Paging::GetPage(u64 address, bool create, PageDirectory* directory)
{
    address /= 0x1000;
    u32 tableIdx = address / 1024;
    if (directory->tables[tableIdx] == nullptr && create)
    {
        u64 physicalLocation = kmalloc(sizeof(PageTable), true);
        directory->tables[tableIdx] = (PageTable*)physicalLocation;
        zero_memory(directory->tables[tableIdx], 0x1000);
        directory->tablesPhysical[tableIdx] = physicalLocation | 0x3;
    }

    if (directory->tables[tableIdx])
        return &directory->tables[tableIdx]->pages[address%1024];

    return nullptr;
}

void Paging::SwitchPageDirectory(PageDirectory* directory)
{
    vga.Print("0x%?\n", (u64)&directory->tablesPhysical);
    vga.Print("%?\n", (u64)directory->tables[0]);
    kpanic("LOL");
    asm volatile("mov %0, %%cr3" :: "r"(&directory->tablesPhysical));
    kpanic("LOL");
}

void Paging::PageFault(const Registers& regs)
{
    kpanic("Page fault!");
}
