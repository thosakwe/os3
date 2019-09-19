.section .text
.global tss_flush
.type tss_flush, @function
// void tss_flush(void* entry_point)
tss_flush:
  # movl 4(%esp), %ebx
  popl %ebx # Store entry point addr in ebx
  mov $0x2b, %ax
  ltr %ax
.tss_flush_enter_ring3:
  mov $0x23, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs # User data seg
  mov %esp, %eax # Push current stack ptr
  push $0x23
  push %eax
  pushf
  push $0x1b # User code seg
  push %ebx
  iret
.size tss_flush, . - tss_flush
