export ARCH=i686
export CLANGXX=clang++
export CXX=$(CLANGXX) --target=$(TARGET) -march=$(ARCH)
export CXXFLAGS+=-ffreestanding -fno-builtin -nostdlib \
	-nostdinc -nostdinc++
export TARGET=$(ARCH)-pc-none-elf
