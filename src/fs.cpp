#include "fs.h"
#include "vga.h"

enum class EntryType : u8
{
    NormalFileFile,
    HardLink,
    SymLink,
    CharDevice,
    BlockDevice,
    Directory,
    Pipe
};

struct TarEntry
{
    union {
        struct {
            char name[100];
            char mode[8];
            char uid[8];
            char gid[8];
            char size[12];
            char mtime[12];
            char check[8];
            EntryType type;
            char also_link_name[100];
            char ustar[8];
            char ustarver[2];
            char ownername[32];
            char groupname[32];
            char major[8];
            char minor[8];
            char filenameprefix[155];
        };

        char block[512];
    };
};

static_assert(sizeof(TarEntry) == 512, "invalid size");

static int oct2bin(const char* str, int size)
{
    int n = 0;
    while (size-- > 0)
    {
        n *= 8;
        n += *str - '0';
        str++;
    }
    return n;
}

TarEntry* g_archive = nullptr;

struct FileNode
{
    const char* name;
    const char* data;
    u32 filesize;
    FileNode* child;
    FileNode* nextSibling;
};

FileNode* root = nullptr;

void fs_init(u32 initrd_start, u32)
{
    g_archive = (TarEntry*)initrd_start;
    for (TarEntry* e = g_archive; e->name[0] != '\0'; e++) 
    {
        int filesize = oct2bin(e->size, 11);
        vga.Print("%? (%? bytes)\n", (const char*)e->name, filesize);
        if (filesize > 0)
        {
            while (filesize > 0)
            {
                filesize -= 512;
                e++;
            }
        }
    }
}


