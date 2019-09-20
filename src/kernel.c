#include <multiboot2.h>
#include <os3_kernel.h>

void kernel_main(unsigned long magic, void *addr) {
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    kputs("Invalid magic given; not Multiboot2. Aborting.");
    return;
  }

  os3_t os3;
  os3.processes = NULL;

  os3_setup_gdt();
  os3_setup_idt();
  os3_setup_paging();

  // Read the Multiboot2 header, in order to find modules.
  struct multiboot_tag *tag = addr + 8;  // Skip size, AND reserved u32 field.
  while (tag->type != MULTIBOOT_TAG_TYPE_END) {
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_MODULE: {
        struct multiboot_tag_module *module =
            (struct multiboot_tag_module *)tag;
        kwrites("Found module (0x");
        kputi_r(module->mod_start, 16);
        kwrites("-0x");
        kputi_r(module->mod_end, 16);
        kwrites("); cmdline=");
        kputs(module->cmdline);
      } break;
    }
  }

  // Create a simple process.
  // os3_process_t* proc = os3_new_process(&os3);
  // proc->entry_point = userspace_demo;
  // os3_enter_process(&os3, proc);
  // kputs("...!");
}
