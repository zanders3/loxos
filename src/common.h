#pragma once

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long s64;
typedef int s32;
typedef short s16;
typedef char s8;
typedef long unsigned int size_t;

//Write/Read from I/O ports
void outb(u16 port, u8 data);
u8 inb(u16 port);
u16 inw(u16 port);
