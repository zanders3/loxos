extern kmain

section .multiboot_header
header_start:
    dd 0xe85250d6                ; magic number
    dd 0                         ; protected mode code
    dd header_end - header_start ; header length

    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
global start
section .text
bits 32
start:
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

	; set callstack and jump to kmain() in 64 bit mode (in main.cpp)
	mov esp, kernel_stack
	jmp gdt64.code:kmain
	hlt

section .bss
align 4096

p4_table:
	resb 4096
p3_table:
	resb 4096
p2_table:
	resb 4096


KERNEL_STACK_SIZE equ 4096
section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE

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
