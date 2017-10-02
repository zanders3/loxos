
section .text
align 4
bits 32
header_start:
	MULTIBOOT_FLAGS equ 1<<0 | 1<<1 ; PAGE_ALIGN, MEMORY_INFO
	MULTIBOOT_MAGIC equ 0x1BADB002
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
global start
start:
	mov esp, kernel_stack_top
	push eax
	push ebx
	extern kmain
	call kmain
	hlt

global gdt_flush
gdt_flush:
	mov eax, [esp+4]
	lgdt [eax]

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush
.flush:
	ret

section .bss
align 4
kernel_stack_bottom: equ $
	resb 16384 ; 16 KB
kernel_stack_top:
