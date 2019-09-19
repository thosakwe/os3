kernel.bin: kernel.o boot.o
	$(LD) -T linker.ld -o $@ -nostdlib $^
