.section .text
.global tss_flush
.type tss_flush, @function
// void tss_flush(void* entry_point)
tss_flush:
  cli
  movl 4(%esp), %ebx
  # popl %ebx # Store entry point addr in ebx
  mov $0x2b, %ax
  ltr %ax
.tss_flush_enter_ring3:
  # IRET:
    # SS
    # ESP
    # EFLAGS
    # CS
    # EIP
  mov $0x23, %ax # Also set RPL to 3 (User data=0x20)
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs # User data seg
  # mov %ax, %ss
  # jmp *%ebx

  push $0x23 # SS (select the same segment as above)
  push %esp # ESP
  pushf # EFLAGS
  push $0x1b # CS User code seg
  push %ebx # ebx has entry_point, the jump address
  iret
.size tss_flush, . - tss_flush
