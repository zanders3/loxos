section .boot
bits 16
global boot
boot:
	mov ax, 0x2401
	int 0x15

	mov [disk],dl

	mov ah, 0x2    ;read sectors
	mov al, [kernel_sector_size];sectors to read
	mov ch, 0      ;cylinder idx
	mov dh, 0      ;head idx
	mov cl, 2      ;sector idx
	mov dl, [disk] ;disk idx
	mov bx, copy_target;target pointer
	int 0x13
	cli
	jmp boot2
gdt_start:
	dq 0x0
gdt_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0
gdt_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
gdt16_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 00001111b
	db 0x0
gdt_end:
gdt_pointer:
	dw gdt_end - gdt_start
	dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
CODE16_SEG equ gdt16_code - gdt_start
disk:
	db 0x0
times 509 - ($-$$) db 0
kernel_sector_size:
	db 0
dw 0xaa55
copy_target:
boot2:
	mov esp,kernel_stack_top
	lgdt [gdt_pointer]
	mov eax, cr0
	or  eax, 0x1
	mov cr0, eax
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	extern kmain
	jmp CODE_SEG:kmain

bits 32
REGSV86_SIZE equ 18
global callv86
callv86:
	pushad
	mov eax, [esp+36]
	mov [.int_no], al
	mov esi, [esp+40]
	sub esp, REGSV86_SIZE
	mov edi, esp
	mov ecx, REGSV86_SIZE
	rep movsb
	jmp word CODE16_SEG:.into_p16
bits 16
.into_p16:
	mov eax, cr0
	and eax,~0x1
	mov cr0, eax
	jmp 0x0:.into_r16
.into_r16:
	mov eax, 0x3
	popa
	pop es
	db 0xCD ;ASM for int
.int_no:
	db 0x0
	mov eax,cr0
	or  eax,0x1
	mov cr0,eax
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp CODE_SEG:.into_p32
bits 32
.into_p32:
	popad
	ret

section .bss
align 4
kernel_stack_bottom: equ $
	resb 16384 ; 16 KB
kernel_stack_top:

