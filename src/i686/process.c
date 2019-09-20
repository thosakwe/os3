#include <os3_kernel.h>
#include "tss.h"

bool os3_enter_process(os3_t* os3, os3_process_t* proc) {
  // Create two GDT entries, for ring 3.
  // One will be a code segment; one will be a data segment.
  // This^ is already done in os3_setup_gdt.

  // Before we switch contexts, we need to create a new page directory
  // for the new process.
  int16_t index = next_page_directory();
  if (index == -1) return false;

  // Map the entry point into an executable region.
  os3_memory_region_t* region =
      (os3_memory_region_t*)kmalloc(sizeof(os3_memory_region_t));
  os3_memory_region_init(region);
  region->access = OS3_MEMORY_EXECUTABLE;
  region->length = proc->entry_point_size;
  region->used = true;
  // Because we have identity-mapped the kernel, the `region` pointer
  // is equivalent to its physical address. Therefore, we can map it
  // into the new page.
  //
  // uint16_t index = 0;
  uint32_t* pd = page_directory;
  os3_page_table_t* pt = page_tables;
  os3_page_table_t* table = &pt[index];
  // os3_init_page_directory(pd, pt, (uint32_t)region, NULL);
  // Set permissions on the new directory.
  // Reset flags, preserve table addr.
  pd[index] &= PAGE_MASK_EMPTY | PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE |
	       PAGE_MASK_RING3;
  // We place the first region at offset 0x0, directly at the start of the
  // page directory. This page can eventually be filled with region
  // structures.
  table->pages[0] = ((uint32_t)region) | PAGE_MASK_PRESENT | PAGE_MASK_RING3;
  // However, since the region and the entry point are not contiguous, we map
  // the entry point into another page.
  region->offset = PAGE_SIZE;
  uint32_t entry_point = (uint32_t)proc->entry_point;
  uint32_t size = 0;
  uint16_t page_index = 1;
  while (size < proc->entry_point_size) {
    uint32_t offset = entry_point + ((page_index - 1) * PAGE_SIZE);
    table->pages[page_index] =
	offset | PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE | PAGE_MASK_RING3;
    page_index++;
    size += PAGE_SIZE;
  }
  // Disable kernel pages.
  // os3_disable_kernel_pages();
  // os3_enable_paging(pd); // This call is unnecessary.

  // Next, set up a TSS with an ESP0 stack.
  // This ASM will fetch ESP into the TSS.
  uint32_t esp;
  asm volatile("mov %%esp, %0" : "=a"(esp));
  tss.esp0 = esp;
  // Flush the TSS. This ASM routine will jump to the given
  // pointer, which in this case is PAGE_SIZE.
  tss_flush();
  usermode_jump(PAGE_SIZE);
  os3_enable_kernel_pages();
  // TODO: Next, set up an IDT for user interrupts???
  // We only enable a few interrupts, for example, syscall 0x80.
  return true;
}

