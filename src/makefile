export CFLAGS+=-I$(shell pwd)/include

.PHONY: $(ARCH)/$(LIBARCH)

kernel.bin: boot.o interrupt_handler.o kernel.o \
	liballoc_1_1.o process.o file_descriptor.o \
	$(ARCH)/$(LIBARCH)
	$(LD) -T linker.ld -o $@ -nostdlib $^

$(ARCH)/$(LIBARCH):
	$(MAKE) -C $(ARCH)
