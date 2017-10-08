#pragma once

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int size_t;

//Write/Read from I/O ports
void outb(u16 port, u8 data);
u8 inb(u16 port);
u16 inw(u16 port);

//Utils
void zero_memory(void* ptr, u32 size);
u32 next_power_of_2(u32 size);
int atoi(const char* val, int valLen);

//Kernel panic
void kpanic_internal(const char* msg, const char* file, int line);

#define kpanic(msg) kpanic_internal(msg, __FILE__, __LINE__)
#define kassert(cond) { if (!(cond)) { kpanic_internal(#cond, __FILE__, __LINE__); } }
