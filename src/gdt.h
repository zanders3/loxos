#pragma once

#include "common.h"

//GDT goes from u32 base to u32 limit
//applying access and ring level flags
struct GDTEntry
{
    u16 limit_low; //limit low 16 bits
    u16 base_low; //base low 16 bits
    u8 base_middle; //base mid 8 bits
    u8 access;// access flags (see init_gdt)
    u8 granularity;//granularity flags (see init_gdt)
    u8 base_high;//base high 8 bits
} __attribute__((packed));

//Pointer to the gdt table defined above used by 
//lgdt instruction
struct GDTPointer
{
    u16 limit;
    u32 base;
} __attribute__((packed));

struct IDTEntry
{
    u16 addr_lo;//target address low 16 bits
    u16 sel;//memory offset to kernel segment selector
    u8 always0;//always 0
    u8 flags;//flags (see init_idt)
    u16 addr_high;//target address high 16 bits
} __attribute__((packed));

struct IDTPointer
{
    u16 limit;
    u32 base;
} __attribute__((packed));

void init_gdt();
void init_idt();
