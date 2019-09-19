export ARCH=i686
export AR=$(ARCH)-elf-ar
export AS=$(ARCH)-elf-as
export CC=$(CLANG) $(CLANGFLAGS)
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

.PHONY: all clean src/kernel.bin run-qemu

all: os3.iso

clean:
	find . \( -name '*.o' -o -name '*.a' \
		-o -name '*.bin' -o -name '*.iso' \) \
		-delete

src/kernel.bin:
	$(MAKE) -C src
	grub-file --is-x86-multiboot $@

.iso/boot/kernel.bin: src/kernel.bin
	mkdir -p .iso/boot
	cp -rp iso/ .iso/
	cp $< $@

os3.iso: .iso/boot/kernel.bin
	grub-mkrescue -o $@ .iso/

run-qemu: os3.iso
	$(QEMU) -cdrom $< -serial file:/dev/stdout
