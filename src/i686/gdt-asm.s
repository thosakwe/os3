# vim: ft=asm
.section .text
.global os3_flush_gdt
.type os3_flush_gdt, @function
os3_flush_gdt:
	# Load the GDT from the stack.
	# movl 4(%esp), %eax
	# popl %eax
	# lgdt (%eax)
	jmp $0x08, $.os3_flush_gdt_end
.os3_flush_gdt_end:
	# Load segment registers.
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ret
# .size os3_flush_gdt, . - os3_flush_gdt
