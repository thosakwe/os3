export CFLAGS+=-I$(shell pwd)/include

.PHONY: $(ARCH)/$(LIBARCH)

kernel.bin: kernel.o boot.o $(ARCH)/$(LIBARCH)
	$(LD) -T linker.ld -o $@ -nostdlib $^

$(ARCH)/$(LIBARCH):
	$(MAKE) -C $(ARCH)
