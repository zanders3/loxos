#pragma once
#include "common.h"

void fs_init(u32 initrd_start, u32 initrd_end);

struct FileInfo
{
    const char* data;
    u32 dataLength;
};
FileInfo fs_readfile(const char* file);
