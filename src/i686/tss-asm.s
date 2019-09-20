.section .text
.global tss_flush
.global usermode_jump
.type tss_flush, @function
.type usermode_jump, @function
// void tss_flush(void* entry_point)
tss_flush:
  mov $0x2b, %ax
  ltr %ax
  ret
.size tss_flush, . - tss_flush
usermode_jump:
  # movl 4(%esp), %ebx
  popl %ebx # Store entry point addr in ebx
  # IRET:
    # SS
    # ESP
    # EFLAGS
    # CS
    # EIP
  cli
  mov $0x23, %ax # Also set RPL to 3 (User data=0x20)
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs # User data seg
  # mov %ax, %ss
  # ljmp *%ebx

  # mov $0x23, %ax
  # mov %ax, %ss
  # mov $0x1b, %ax
  # mov %ax, %cs
  # sti
  # jmp *%ebx

  pushl $0x23 # SS (select the same segment as above)
  pushl %esp # ESP
  pushf # EFLAGS

  # Re-enable interrupts after IRET:
  # pop %eax
  # push %ebx
  # mov $0x200, %ebx
  # or %eax, %ecx
  # pop %ebx
  # push %eax

  pushl $0x1b # CS User code seg
  push %ebx # ebx has entry_point, the jump address
  iret
.size usermode_jump, . - usermode_jump
