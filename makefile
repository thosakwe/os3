export ARCH=i686
export AS=$(ARCH)-elf-as
export CC=$(CLANG) $(CLANGFLAGS)
export CLANG=clang
export CLANGXX=clang++
export CLANGFLAGS=--target=$(TARGET) -march=$(ARCH)
export CXX=$(CLANGXX) $(CLANGFLAGS)
export CXXFLAGS+=-ffreestanding -fno-builtin -nostdlib \
	-nostdinc -nostdinc++ -fno-rtti
export LD=$(ARCH)-elf-ld
export TARGET=$(ARCH)-pc-none-elf

.PHONY: all clean src/kernel.bin

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
