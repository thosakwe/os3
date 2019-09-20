#include <multiboot2.h>
#include <os3_kernel.h>

void kernel_main(unsigned long magic, void *addr) {
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    kputs("Invalid magic given; not Multiboot2. Aborting.");
    return;
  }

  uint32_t ram_start = 0x0, ram_end = (uint32_t)&endkernel;
  os3_t os3;
  os3.processes = NULL;
  os3_setup_gdt();
  os3_setup_idt();

  // Read the Multiboot2 header, in order to find modules.
  struct multiboot_tag *tag = addr + 8;  // Skip size, AND reserved u32 field.
  kwrites("Multiboot2 tag addr: 0x");
  kputi_r((uint32_t)tag, 16);
  kputc('\n');
  while (tag->type != MULTIBOOT_TAG_TYPE_END) {
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE: {
        struct multiboot_tag_string *cmdline =
            (struct multiboot_tag_string *)tag;
        kwrites("Boot command line: ");
        kputs(cmdline->string);
      } break;
      case MULTIBOOT_TAG_TYPE_MMAP: {
        struct multiboot_tag_mmap *mmap_tag = (struct multiboot_tag_mmap *)tag;
        multiboot_memory_map_t *mmap = mmap_tag->entries;
        while (mmap < (multiboot_memory_map_t *)(tag + tag->size)) {
          if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // TODO: Handle multiple available RAM.
            ram_start = mmap->addr;
            // ram_end = ram_start + mmap->len;
            // Print info.
            kwrites("Found available RAM at 0x");
            kputi_r(mmap->addr, 16);
            // TODO: Why did the GRUB example include these shifts, etc.?
            // kputi_r(mmap->addr >> 3, 16);
            // kputi_r(mmap->addr & 0xffffffff, 16);
            kwrites(" of size 0x");
            kputi_r(mmap->len, 16);
            kwrites("; type=");
            kputi(mmap->type);
            kputc('\n');
          }
          // Jump to the next.
          mmap += mmap_tag->entry_size;
        }
      } break;
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
    // Jump to the next one.
    tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                   ((tag->size + 7) & ~7));
  }

  // We need to be able to access the multiboot information still,
  // so if it's out of range, expand ram_end to include it.
  tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                 ((tag->size + 7) & ~7));
  uint32_t tag_ptr = (uint32_t)tag;
  if (tag_ptr > ram_end) ram_end = tag_ptr;
  os3_setup_paging(ram_start, ram_end);
  kwrites("Endkernel: 0x");
  kputi_r((uint32_t)&endkernel, 16);
  kputc('\n');

  // Create a simple process.
  // os3_process_t* proc = os3_new_process(&os3);
  // proc->entry_point = userspace_demo;
  // os3_enter_process(&os3, proc);
  // kputs("...!");
}
