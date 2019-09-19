#include <os3_kernel.h>

extern void isr13();

void kernel_main(void) {
  os3_t os3;
  os3.processes = NULL;

  os3_setup_gdt();
  kputs("GDT up");
  os3_setup_idt();
  kputs("IDT up");
  os3_setup_paging();
  kputs("Paging up?");
}
