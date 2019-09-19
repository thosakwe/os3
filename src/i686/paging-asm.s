.section .text
.global os3_enable_paging
.type os3_enable_paging, @function

os3_enable_paging:
	# Load the page directory into CR3.
	mov 4(%esp), %eax
	mov %eax, %cr3

	# Bitwise or the contents of CR0 with the value
	# 0x80000001, which enables the paging (PG) and
	# protection (PE) bits.
	mov %cr0, %eax
	or $0x80000001, %eax
	mov %eax, %cr0

	ret

.size os3_enable_paging, . - os3_enable_paging
