default: run

.PHONY: clean

ASMS := $(shell find src -name '*.asm')
CPPS := $(ASMS:src/%.asm=build/%.o) $(shell find src -name '*.cpp')
OPTIONS := -Isrc -nostdlib -fno-exceptions -mno-red-zone -mno-ms-bitfields -ffreestanding -mgeneral-regs-only -std=c++11 -fno-rtti -Wall -Wextra -Werror

build/writekernelsize: writekernelsize.cpp
	mkdir build
	g++ writekernelsize.cpp -o build/writekernelsize

build/%.o: src/%.asm
	nasm -f elf32  $< -o $@

build/kernel.bin: build/writekernelsize $(CPPS) src/linker.ld $(shell find src -name '*.h')
	i386-elf-g++ $(CPPS) -O3 -o $@ -T src/linker.ld $(OPTIONS)
	build/writekernelsize

run: build/kernel.bin
	DISPLAY=:0 qemu-system-i386 -fda build/kernel.bin

build: build/kernel.bin
	
clean:
	rm -rf build
