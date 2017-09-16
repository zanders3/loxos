section .interrupts
bits 64

; Define IDT code for 255 interrupt handlers - each pushes the interrupt idx
; then jumps to .handle
interrupts:
.first:
	push qword 0
	jmp qword .handle
.second:
%assign i 1
%rep 255
	push qword i
	jmp qword .handle
%assign i i+1
%endrep

.handle:
	push rbp ; Save all registers
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rdx
	push rcx
	push rbx
	push rax

	mov rdi, rsp ; Save stack pointer
	push rdi
	mov rdi, rsp

	extern kfault_handler
	call kfault_handler ; Call fault handler

	pop rsp ; Pop stack pointer

	pop rax ; Restore all registers
	pop rbx
	pop rcx
	pop rdx
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop rbp
	add rsp, 8 ; pop error code qword
	sti
	iretq

; IDTR definition
global idtr
idtr:
	dw (idt.end - idt) + 1
	dq idt

%define BASE_OF_SECTION 0x104000 ;IF INTERRUPTS EXPLODE - it is because the linker has relocated the interrupts!
%define SIZE_OF_INTCODE (interrupts.second-interrupts.first)

; IDT definition
extern gdt64.code
idt:
%assign i 0
%rep 255
;((interrupts.first + ((interrupts.second-interrupts.first)*i)) & 0xFFFF)
	dw ((BASE_OF_SECTION + (SIZE_OF_INTCODE*i)) & 0xFFFF) ; offset to low bits
	dw gdt64.code ; pointer to gdt code segment selector
	db 0
	db (1<<7) | 0xE ; PRESENT | INTERRUPT64
	dw ((BASE_OF_SECTION + (SIZE_OF_INTCODE*i)) >> 16) ; offset to middle bits
	dd 0 ; offset to higher bits
	dd 0 ; always 0
%assign i i+1
%endrep
.end ; IDT end
