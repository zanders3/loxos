#include "kalloc.h"
#include "paging.h"
#include "vga.h"

int g_allocatedSlabs, g_totalSlabs, g_allocCount = 0;

struct SlabEntry
{
    SlabEntry* next;
};

struct Slab
{
    void Init(u32 slabStart, u16 size, bool isMeta)
    {
        m_nextSlab = nullptr;
        m_slabStart = slabStart;
        m_size = size;

        kassert(size >= sizeof(SlabEntry));

        map_page(slabStart, PageFlags::RW);
        zero_memory((void*)slabStart, 0x1000);

        vga.Print("%?/%? total slabs (%?%?)\n", ++g_allocatedSlabs, g_totalSlabs, (int)size, isMeta ? " META" : "");

        u32 numEntries = (0x1000 / size) - 1;
        m_freelist = (SlabEntry*)slabStart;
        SlabEntry* current = m_freelist;
        for (u32 i = 1; i<numEntries; i++)
        {
            current->next = (SlabEntry*)(slabStart + (i * size));
            current = current->next;
        }

        kassert(current->next == nullptr);
        kassert(((u32)current + size) < (slabStart + 0x1000));
        kassert((u32)m_freelist == slabStart);
    }

    bool Alloc(u32 size, u32& newLoc)
    {
        if (m_size != size || m_freelist == nullptr)
            return false;

        newLoc = (u32)m_freelist;
        if (newLoc < m_slabStart || newLoc > m_slabStart + 0x1000)
            kpanic("heap corruption");

        m_freelist = m_freelist->next;
        return true;
    }

    bool Free(u32 location)
    {
        if (location < m_slabStart || location >= m_slabStart + 0x1000)
            return false;
        
        //check for size alignment
        {
            u32 alignedLoc = (location / m_size) * m_size;
            if (alignedLoc != location)
                kpanic("invalid free location");
        }

        //check for double free and heap corruption
        for (SlabEntry* entry = m_freelist; entry; entry = entry->next)
        {
            if (entry && ((u32)entry < m_slabStart || (u32)entry > m_slabStart + 0x1000))
                kpanic("heap corruption");
            if (location == (u32)entry)
                kpanic("double free");
            else if (location == (u32)entry->next)
                kpanic("not allocated");
        }

        SlabEntry* newEntry = (SlabEntry*)location;
        newEntry->next = m_freelist;
        m_freelist = newEntry;
        return true;
    }

    inline u32 GetSize() { return m_freelist ? m_size : 0; }

    Slab* m_nextSlab;
private:
    SlabEntry* m_freelist;
    u32 m_slabStart;
    u16 m_size;
};

u32 g_memStart, g_memEnd;
Slab* g_slabList;
Slab* g_slabMetaData;

//creates a slab that allocates slab metadata
//the metadata for the slab itself is allocated in first metadata slot
static Slab* alloc_slab_meta(u32 slabStart)
{
    Slab slabMetadata;
    slabMetadata.Init(slabStart, sizeof(Slab), true);
    u32 slabLoc;
    bool didAlloc = slabMetadata.Alloc(sizeof(Slab), slabLoc);
    kassert(didAlloc);
    kassert(slabStart == slabLoc);

    Slab* newSlabMeta = (Slab*)slabLoc;
    *newSlabMeta = slabMetadata;
    return newSlabMeta;
}

void kalloc_init(u32 memStart, u32 memSize)
{
    g_memStart = memStart;
    g_memEnd = memStart + memSize;
    g_slabList = nullptr;
    g_allocatedSlabs = 0;
    g_totalSlabs = memSize / 0x1000;

    g_slabMetaData = alloc_slab_meta(g_memStart);
    g_memStart += 0x1000;
    g_allocCount = 0;
}

void* kalloc(u32 size, bool exactFit)
{
    ++g_allocCount;

    if (size < sizeof(SlabEntry))
        size = sizeof(SlabEntry);

    u32 newLoc;
    if (!exactFit)
        size = next_power_of_2(size);
    
    Slab* slab = g_slabList;
    for (; slab; slab = slab->m_nextSlab)
    {
        if (slab->Alloc(size, newLoc))
        {
            return (void*)newLoc;
        }
    }

    kassert(g_memStart < g_memEnd);//out of memory!

    u32 slabLoc;
    bool didAlloc = g_slabMetaData->Alloc(sizeof(Slab), slabLoc);
    if (didAlloc == false)
    {
        vga.Print("new meta\n");
        g_slabMetaData = alloc_slab_meta(g_memStart);
        g_memStart += 0x1000;
        didAlloc = g_slabMetaData->Alloc(sizeof(Slab), slabLoc);
    }
    kassert(didAlloc);

    Slab* newSlab = (Slab*)slabLoc;
    newSlab->Init(g_memStart, size, false);
    g_memStart += 0x1000;
    newSlab->m_nextSlab = g_slabList;
    g_slabList = newSlab;

    didAlloc = newSlab->Alloc(size, newLoc);
    kassert(didAlloc);
    return (void*)newLoc;
}

void kfree(void* ptr)
{
    if (ptr == nullptr)
        return;

    --g_allocCount;
    u32 loc = (u32)ptr;
    for (Slab* slab = g_slabList; slab; slab = slab->m_nextSlab)
        if (slab->Free(loc))
            return;

    kpanic("pointer not in heap");
}

int kalloc_count()
{
    return g_allocCount;
}

KAllocator kallocator;
