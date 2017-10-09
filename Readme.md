# LoxOS

A tiny hobby operating system 

## Features

- C++
- Protected mode 32 bit x86 OS with virtual memory and heap memory allocation
- Simple shell interface, keyboard and vga text mode driver
- Interprets the lox programming language from http://craftinginterpreters.com/

## Installation

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
