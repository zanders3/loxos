section .text
align 4
bits 32

%macro ISR_ERRCODE 1
.isr%1:
    cli
    push byte %1
    jmp isr_common_handler
%endmacro
%macro ISR_NOERRCODE 1
.isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_handler
%endmacro
%macro IRQ 1
.irq%1:
    cli
    push byte 0
    push byte %1
    jmp irq_common_handler
%endmacro

; Interrupts location table, used in isr.cpp to fill the IDT
global isr_loc
isr_loc:
;ISRs 0 -> 32
%assign i 0
%rep 32
dd isrs.isr%+i
%assign i i+1
%endrep
;IRQs 0 -> 15
%assign i 0
%rep 16
dd isrs.irq%+i
%assign i i+1
%endrep

; Using the macros above define each interrupt handler
isrs:
;ISRs 0  -> 14
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2 
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
;ISRs 15 -> 32
%assign i 15
%rep 17
ISR_NOERRCODE i
%assign i i+1
%endrep
;IRQs 0-15
%assign i 0
%rep 16
IRQ i
%assign i i+1
%endrep

;Common handler for all CPU interrupts
isr_common_handler:
	pusha
	mov ax, ds
	push eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	extern isr_handler
	call isr_handler

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8
	iret

;Common handler for all IRQs
irq_common_handler:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern irq_handler
    call irq_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret
