#include <os3_kernel.h>

void userspace_demo();

void kernel_main(void) {
  os3_t os3;
  os3.processes = NULL;

  os3_setup_gdt();
  os3_setup_idt();
  os3_setup_paging();

  // Create a simple process.
  os3_process_t* proc = os3_new_process(&os3);
  proc->entry_point = userspace_demo;
  os3_enter_process(&os3, proc);
  kputs("...");
}

void userspace_demo(int argc, char** argv) {
  // kputs("This should never happen.");
  asm("int $0x80");
}
