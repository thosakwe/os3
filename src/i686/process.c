#include <os3_kernel.h>
#include "paging.h"
#include "tss.h"

bool os3_enter_process(os3_t* os3, os3_process_t* proc) {
  // Create two GDT entries, for ring 3.
  // One will be a code segment; one will be a data segment.
  // This^ is already done in os3_setup_gdt.

  // Before we switch contexts, we need to create a new page directory
  // for the new process.
  int16_t index = next_page_directory();
  if (index == -1) return false;

  // Set permissions on the new directory.
  // Reset flags, preserve table addr.
  page_directory[index] &= PAGE_MASK_EMPTY | PAGE_MASK_PRESENT |
			   PAGE_MASK_READ_WRITE | PAGE_MASK_RING3;
	// Map the entry point into an executable region.

  // Next, set up a TSS with an ESP0 stack.
  // This ASM will fetch ESP into the TSS.
  uint32_t esp;
  asm volatile("mov %%esp, %0" : "=a"(esp));
  tss.esp0 = esp;
  // Flush the TSS.
  tss_flush(proc->entry_point);
  // asm ("mov $0x2b, %ax");
  // asm("ltr %ax");
  // TODO: Next, set up an IDT for user interrupts???
  // We only enable a few interrupts, for example, syscall 0x80.
  return true;
}

