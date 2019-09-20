.set MULTIBOOT2_HEADER_MAGIC, 0xe85250d6
.set GRUB_MULTIBOOT_ARCHITECTURE_I386, 0

# Multiboot2
.section .multiboot2
.align 8
multiboot_start:
.long MULTIBOOT2_HEADER_MAGIC
.long GRUB_MULTIBOOT_ARCHITECTURE_I386
.long multiboot_end - multiboot_start  # Header length
# Checksum
.long -(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + (multiboot_end - multiboot_start))

# Close off the tag list.
.short 0
.short 0
.long 8
multiboot_end:

# Make a litle stack.
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

# Entry
.section .text
.global _start
.type _start, @function
_start:
	# There is stuff to do before entering the kernel.
	mov $stack_top, %esp
	# Reset EFlags.
	push $0
	popf

	# Push the Multiboot info struct, and the magic.
	push %ebx
	push %eax

	# Kernel!
	cli # Disable interrupts, so we can enter protected mode.
	call kernel_main
	# Forever loop.
	cli
1:	hlt
	jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
