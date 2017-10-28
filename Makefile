default: run

.PHONY: clean

ASMS := $(shell find src -name '*.asm')
CPPS := $(ASMS:src/%.asm=build/%.o) $(shell find src -name '*.cpp')
HDRS := $(shell find src -name '*.h')
OPTIONS := -O2 -Isrc -nostdlib -fno-exceptions -mno-red-zone -mno-ms-bitfields -ffreestanding -mgeneral-regs-only -std=c++11 -fno-rtti

build/writekernelsize: writekernelsize.cpp
	mkdir -p build
	g++ writekernelsize.cpp -o build/writekernelsize

build/%.o: src/%.asm
	nasm -f elf32  $< -o $@

build/kernel.bin: build/writekernelsize $(CPPS) src/linker.ld $(HDRS)
	i386-elf-g++ $(CPPS) -o $@ -T src/linker.ld $(OPTIONS) -Wall -Wextra -Werror
	build/writekernelsize

run: build/kernel.bin
	DISPLAY=:0 qemu-system-i386 -drive format=raw,file=build/kernel.bin,index=0,if=floppy -serial stdio

build: build/kernel.bin
	
clean:
	rm -rf build
