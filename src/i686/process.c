#include <os3_kernel.h>
#include "tss.h"

bool os3_enter_process(os3_t* os3, os3_process_t* proc) {
  // Get a free page.
  // Compute the total needed size.
  size_t total_size = sizeof(os3_memory_region_t) + proc->entry_point_size;
  size_t pages_needed = (total_size / PAGE_SIZE) + 1;
  kputs("a");
  void* page = liballoc_alloc(pages_needed);
  if (page == NULL) return false;

  // Mark the pages as ring3.
  kputs("b");
  uint16_t page_index = ((uint16_t)page) / PAGE_SIZE;
  for (size_t i = 0; i < pages_needed; i++) {
    page_directory[0] |= PAGE_MASK_RING3;
    page_tables[0].pages[i] |= PAGE_MASK_RING3;
  }

  // Treat the start of the page as a memory region.
  kputs("c");
  os3_memory_region_t* region = page;
  os3_memory_region_init(region);
  region->access = OS3_MEMORY_EXECUTABLE;
  region->offset = sizeof(os3_memory_region_t);
  region->length = proc->entry_point_size;
  region->used = true;

  // Next, copy the entry point into the page.
  void* entry_point = page + region->offset;
  kputs("d");
  kmemcpy(entry_point, proc->entry_point, proc->entry_point_size);

  // Next, set up a TSS with an ESP0 stack.
  // This ASM will fetch ESP into the TSS.
  kputs("e");
  uint32_t esp;
  asm volatile("mov %%esp, %0" : "=a"(esp));
  tss.esp0 = esp;
  // Flush the TSS. This ASM routine will jump to the given
  // pointer, which in this case is PAGE_SIZE.
  kputs("f");
  tss_flush();
  // Jump to the entry_point offset. The permissions *should*
  // be correct?
  kputs("g");
  usermode_jump((uint32_t)entry_point);
  kputs("h");
  // Disable kernel pages.
  // os3_disable_kernel_pages();
  // usermode_jump(PAGE_SIZE);
  // os3_enable_kernel_pages();
  // TODO: Next, set up an IDT for user interrupts???
  // We only enable a few interrupts, for example, syscall 0x80.
  return true;
}

