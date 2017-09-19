# LoxOS

Tiny OS

	sudo apt-get install nasm xorriso qemu build-essential
	sudo apt-get install grub-pc-bin
	export DISPLAY=:0
	make

Also need https://sourceforge.net/projects/xming/ installed for Windows 10.

## Debugging

	gdb -ex "target remote localhost:1234" -ex "symbol-file build/kernel.bin"

## Description

- Starts in start: in boot.asm
- Jumps to kmain in main.cpp
- main.cpp sets up interrupts and the PIC and the VGA display
