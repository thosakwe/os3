export CFLAGS+=-I$(shell pwd)/include

$(LIBARCH): gdt.o idt.o kprint.o paging.o \
	paging-asm.o idt-asm.o gdt-asm.o process.o \
	tss-asm.o
	$(AR) rcs $@ $^
