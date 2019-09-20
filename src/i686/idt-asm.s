# vim: ft=asm
.section .text

.macro isr no
	.global isr\no
	.type isr\no, @function
	isr\no:
		pushl $0 
		pushl $\no 
		jmp interrupt_setup
	.size isr\no, . - isr\no
.endm

.macro isr_err no index
	.global isr\no
	.type isr\no, @function
	isr\no:
		pushl $\no 
		jmp interrupt_setup
	.size isr\no, . - isr\no
.endm

.macro irq no
	.global irq\no
	.type irq\no, @function
	irq\no:
		pushl $0 # err_code
		pushl $(\no + 32) # int_no
		jmp interrupt_setup
	.size irq\no, . - irq\no
.endm

.global get_page_fault_pointer
.type get_page_fault_pointer, @function
get_page_fault_pointer:
	mov %cr2, %eax
	ret
.size get_page_fault_pointer, . - get_page_fault_pointer

.global os3_flush_idt
.type os3_flush_idt, @function
os3_flush_idt:
	# Load the IDT from the stack.
	# movl 4(%esp), %eax
	# popl %eax
	# lidt (%eax)
	# sti
	ret

isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr_err 8
isr 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14
isr 15
isr 16
isr 17
isr 18
isr 19
isr 20
isr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
isr 29
isr 30
isr 31
isr 32
isr 128 # Syscall interrupt

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

.type interrupt_setup, @function
interrupt_setup:
	# Push registers, etc.
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi

	movl %ds, %ebp
	pushl %ebp
	movl %es, %ebp
	pushl %ebp
	movl %fs, %ebp
	pushl %ebp
	movl %gs, %ebp
	pushl %ebp

	movl $0x10, %ebp
	movl %ebp, %ds
	movl %ebp, %es
	movl %ebp, %fs
	movl %ebp, %gs

	movl %cr2, %ebp
	pushl %ebp

	movl %esp, %ebx
	subl $4, %esp
	andl $0xFFFFFFF0, %esp # Bitwise operation to align the stack!
	movl %ebx, (%esp)
	call interrupt_handler
	movl %ebx, %esp

	popl %ebp
	movl %ebp, %cr2

	popl %ebp
	movl %ebp, %gs
	popl %ebp
	movl %ebp, %fs
	popl %ebp
	movl %ebp, %es
	popl %ebp
	movl %ebp, %ds

	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	popl %edx
	popl %ecx
	popl %eax

	addl $8, %esp
	iret
.size interrupt_setup, . - interrupt_setup
