.section .text
.global tss_flush
.global usermode_jump
.global os3_usermode_exit
.global os3_usermode_exit_end
.type tss_flush, @function
.type usermode_jump, @function
.type os3_usermode_exit, @function

# void tss_flush(void* entry_point)
tss_flush:
  mov $0x2b, %ax
  ltr %ax
  ret
.size tss_flush, . - tss_flush
usermode_jump:
  # movl 4(%esp), %ebx
  popl %ecx # Store address of usermode_exit
  popl %ebx # Store entry point addr in ebx
  # Save the kernel stack.
  mov %esp, (last_kernel_stack)
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
  # pushl %esp # ESP
  pushl %ecx
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

last_kernel_stack:
  .long 0

os3_usermode_exit:
  # When a usermode process exits, return to ring 0.
  # Set CS_MSR(174h) to our code segment.
  mov $0x174, %ecx
  mov $0, %edx
  mov $0x08, %eax
  wrmsr
  # Write ESP_MSR(175h) to our last kernel stack.
  mov $0x175, %ecx
  mov $0, %edx
  mov (last_kernel_stack), %eax
  wrmsr
  # Write EIP_EIP(176h) to our sysenter_entry.
  mov $0x176, %ecx
  mov $0, %edx
  mov (os3_sysenter_entry), %eax
  wrmsr
  sysenter
.size os3_usermode_exit, . - os3_usermode_exit
os3_usermode_exit_end:
  .byte 0
