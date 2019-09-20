export ARCH=i686
export AR=$(ARCH)-elf-ar
export AS=$(ARCH)-elf-as
export CC=$(CLANG) $(CLANGFLAGS)
export CFLAGS+=-std=gnu99
export CLANG=clang
export CLANGXX=clang++
export CLANGFLAGS=--target=$(TARGET) -march=$(ARCH)
export CXX=$(CLANGXX) $(CLANGFLAGS)
export CXXFLAGS+=-ffreestanding -fno-builtin -nostdlib \
	-nostdinc++ -fno-rtti
export LD=$(ARCH)-elf-ld
export LIBARCH=lib$(ARCH).a
export QEMU=qemu-system-i386
export TARGET=$(ARCH)-pc-none-elf

.PHONY: all bochs clean modules src/kernel.bin run-qemu

all: os3.iso

debug: CFLAGS+= -g -DOS3_DEBUG=1
debug: all

bochs: debug
	bochs -q -rc debug.rc

clean:
	find . \( -name '*.o' -o -name '*.a' \
		-o -name '*.bin' -o -name '*.iso' \) \
		-delete

modules:
	$(MAKE) -C modules

src/kernel.bin:
	$(MAKE) -C src
	grub-file --is-x86-multiboot2 $@

.isodir/boot/kernel.bin: src/kernel.bin
	mkdir -p .isodir/boot
	mkdir -p .isodir/modules
	cp -rp iso/ .isodir/
	# cp modules/*.mod .isodir/modules
	cp $< $@

os3.iso: .isodir/boot/kernel.bin
	grub-mkrescue -o $@ .isodir/

run-qemu: os3.iso
	$(QEMU) -cdrom $< -serial file:/dev/stdout
