default: run

.PHONY: clean

OBJS := build/boot.o build/main.o build/vga.o build/common.o build/paging.o build/gdt.o build/isr.o build/interrupts.o
OPTIONS := -m32 -nostdlib -fno-exceptions -mno-red-zone -mno-ms-bitfields -ffreestanding -std=c++11 -fno-rtti -Wall -Wextra

build/%.o: src/%.asm
	mkdir -p build
	nasm -f elf32 $< -o $@

build/%.o: src/%.cpp
	g++ -g -c $< $(OPTIONS) $(sources) -o $@

build/kernel.bin: $(OBJS) src/linker.ld
	g++ -n -o $@ -T src/linker.ld $(OPTIONS) $(OBJS)

build/os.iso: build/kernel.bin src/grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp src/grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot/
	grub-mkrescue -o build/os.iso build/isofiles

run: build/os.iso
	DISPLAY=:0 qemu-system-x86_64 -cdrom build/os.iso

build: build/os.iso

debug: build/os.iso build/kernel.bin
	DISPLAY=:0 qemu-system-x86_64 -s -S -cdrom build/os.iso

clean:
	rm -rf build
