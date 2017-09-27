#pragma once

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int size_t;

//Write/Read from I/O ports
void outb(u16 port, u8 data);
u8 inb(u16 port);
u16 inw(u16 port);

void kpanic(const char* msg);
void zero_memory(void* ptr, u32 size);
