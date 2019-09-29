#include "tss.h"
#include <os3_kernel.h>

bool os3_enter_process(os3_t *os3, os3_process_t *proc) {
  // Get a free page.
  // Compute the total needed size.
  size_t total_size = sizeof(os3_memory_region_t) + proc->entry_point_size;
  // Also include space for our return code.
  total_size +=
      ((uint32_t)&os3_usermode_exit_end) - (uint32_t)&os3_usermode_exit;
  size_t pages_needed = (total_size / PAGE_SIZE) + 1;
  // void *page = liballoc_alloc(pages_needed);
  void *page = os3_get_pages(pages_needed, PAGE_MASK_RING3);
  if (page == NULL) {
    kwrites("Failed to alloc pages for process: ");
    kputi(pages_needed);
    kputc('\n');
    return false;
  }

  // Mark the pages as ring3.
  // os3_page_set_flags(page, pages_needed, PAGE_MASK_RING3);
  // uint16_t page_index = ((uint16_t)page) / PAGE_SIZE;
  // for (size_t i = page_index; i < pages_needed; i++) {
  //   // for (size_t i = 0; i < page_index + pages_needed; i++) {
  //   page_directory[0] |= PAGE_MASK_RING3;
  //   page_tables[0].pages[i] |= PAGE_MASK_RING3;
  // }

  // Treat the start of the page as a memory region.
  os3_memory_region_t *region = page;
  os3_memory_region_init(region);
  region->access = OS3_MEMORY_EXECUTABLE;
  region->offset = sizeof(os3_memory_region_t);
  region->length = proc->entry_point_size;
  region->used = true;

  // Next, copy the entry point into the page.
  void *entry_point = page + region->offset;
  kmemcpy(entry_point, proc->entry_point, proc->entry_point_size);
  kputptr("Mapped proc to", page);
  kputptr("Extent of mapped memory", entry_point + proc->entry_point_size);

  // // Fill the rest of the region with NOP (0x90).
  // uint32_t regionEnd = ((uint32_t)entry_point) + proc->entry_point_size;
  // uint32_t nopLength = (pages_needed * PAGE_SIZE) - regionEnd;
  // kmemset((void *)regionEnd, 0x90, nopLength);
  // return true;

  // Next, set up a TSS with an ESP0 stack.
  // This ASM will fetch ESP into the TSS.
  uint32_t esp;
  asm volatile("mov %%esp, %0" : "=a"(esp));
  tss.esp0 = esp;
  // Flush the TSS. This ASM routine will jump to the given
  // pointer, which in this case is PAGE_SIZE.
  tss_flush();
  // Jump to the entry_point offset. The permissions *should*
  // be correct?
  usermode_jump((uint32_t)entry_point, (uint32_t)&os3_usermode_exit);
  // Disable kernel pages.
  // os3_disable_kernel_pages();
  // usermode_jump(PAGE_SIZE);
  // os3_enable_kernel_pages();
  // TODO: Next, set up an IDT for user interrupts???
  // We only enable a few interrupts, for example, syscall 0x80.
  return true;
}

