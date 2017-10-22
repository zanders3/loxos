#pragma once

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;


void outb(u16 port, u8 value);
u8 inb(u16 port);

void idt_setup(int i, unsigned int address);
inline void idt_irq_setup(int i, unsigned int address) { idt_setup(i + 32, address); }
void idt_irq_ack(int irq);
void idt_init();
