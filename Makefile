default: run

.PHONY: clean

OBJS := build/boot.o build/interrupts.o $(shell find src -name '*.cpp')
OPTIONS := -Isrc -m32 -nostdlib -fno-exceptions -mno-red-zone -mno-ms-bitfields -ffreestanding -std=c++11 -fno-rtti -Wall -Wextra -Werror

build/%.o: src/%.asm
	mkdir -p build
	nasm -f elf32 $< -o $@

build/kernel.bin: $(OBJS) src/linker.ld $(shell find src -name '*.h')
	g++ -g -n -o $@ -T src/linker.ld $(OPTIONS) $(OBJS)

build/disk.tar: $(shell find disk)
	tar -cf build/disk.tar disk/

build/os.iso: build/kernel.bin src/grub.cfg build/disk.tar
	mkdir -p build/isofiles/boot/grub
	cp src/grub.cfg build/isofiles/boot/grub
	cp build/disk.tar build/isofiles/boot/
	cp build/kernel.bin build/isofiles/boot/
	grub-mkrescue -o build/os.iso build/isofiles

run: build/os.iso
	DISPLAY=:0 qemu-system-x86_64 -cdrom build/os.iso

build: build/os.iso

debug: build/os.iso build/kernel.bin
	DISPLAY=:0 qemu-system-x86_64 -s -S -cdrom build/os.iso

clean:
	rm -rf build
