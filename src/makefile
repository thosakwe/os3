export CFLAGS+=-I$(shell pwd)/include

kernel.bin: kernel.o boot.o
	$(LD) -T linker.ld -o $@ -nostdlib $^
