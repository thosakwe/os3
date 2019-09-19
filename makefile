export ARCH=i686
export AS=$(ARCH)-elf-as
export CC=$(CLANG) $(CLANGFLAGS)
export CLANG=clang
export CLANGXX=clang++
export CLANGFLAGS=--target=$(TARGET) -march=$(ARCH)
export CXX=$(CLANGXX) $(CLANGFLAGS)
export CXXFLAGS+=-ffreestanding -fno-builtin -nostdlib \
	-nostdinc -nostdinc++
export LD=$(ARCH)-elf-ld
export TARGET=$(ARCH)-pc-none-elf

.PHONY: all clean src/kernel.bin

all: src/kernel.bin

clean:
	find . \( -name '*.o' -o -name '*.a' \
		-o -name '*.bin' -o -name '*.iso' \) \
		-delete

src/kernel.bin:
	$(MAKE) -C src
