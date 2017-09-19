extern kmain

section .multiboot_header
align 4
header_start:
	MULTIBOOT_FLAGS equ 1<<0 | 1<<1 ; PAGE_ALIGN, MEMORY_INFO
	MULTIBOOT_MAGIC equ 0x1BADB002
	MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
header_end:
global start
section .text
bits 32
start:
	mov dword [multiboot_magic], eax
	mov dword [multiboot], ebx

	; Page table setup
	mov eax, p3_table ; Point L4 table to L3 table
	or eax, 0b11 ; set page in memory and page can be written to
	mov dword [p4_table], eax
	mov eax, p2_table ; Point L3 table to L2 table
	or eax, 0b11
	mov dword [p3_table], eax
	mov ecx, 0

	.map_p2_table:
	mov eax, 0x20000 ; 2MiB
	mul ecx ;eax *= ecx
	or eax, 0b10000011 ; first 1 is huge page bit
	mov [p2_table + ecx * 8], eax

	inc ecx
	cmp ecx, 512
	jne .map_p2_table

	; page table to cr3
	mov eax, p4_table
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; set long mode
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	or eax, 1 << 16
	mov cr0, eax

	; load GDT
	lgdt [gdt64.pointer]

	; load IDTR
	extern idtr ; from interrupts.asm
	lidt [idtr]

	; update selectors
	mov ax, gdt64.data
	mov ss, ax
	mov ds, ax
	mov es, ax

	jmp gdt64.code:longmode ; long jump to 64bit!
	hlt

bits 64
longmode:
	; set callstack and jump to kmain() in 64 bit mode (in main.cpp)
	mov rsp, kernel_stack_top
	jmp kmain
	hlt

section .bss
align 4096

p4_table:
	resb 4096
p3_table:
	resb 4096
p2_table:
	resb 4096

section .bss
global multiboot
multiboot:
	resb 4
global multiboot_magic
multiboot_magic:
	resb 4

align 4
kernel_stack_bottom: equ $
	resb 16384 ; 16 KB
kernel_stack_top:

section .rodata
global gdt64.code
gdt64:
	dq 0
.code: equ $ - gdt64
	dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
.data: equ $ - gdt64
	dq (1<<44) | (1<<47) | (1<<41)
.pointer:
	dw .pointer - gdt64 - 1
	dq gdt64
