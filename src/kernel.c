#include <os3_kernel.h>

extern void isr13();

void kernel_main(void) {
  os3_t os3;
  os3.processes = NULL;

  os3_setup_gdt();
  kputs("GDT up");
  os3_setup_idt();
  kputs("IDT up");
  // os3_setup_paging();
  // kwrites("isr13: 0x");
  // kputi_r((uint32_t)&isr13, 16);
  // kputc('\n');

  // asm("int $0x6");
  // asm("int $0xf");
  // asm("int $0x23");
  // kwrites("a");
  // asm("int $0x5");
  // asm("int $0x0d");
  // asm("int $0x80");
  // kwrites("b");
}
