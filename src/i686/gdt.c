#include "gdt.h"
#include <os3_kernel.h>
#include "tss.h"

gdt_descriptor_t gdt_descriptor;
gdt_entry_t gdt[6];
os3_tss_entry_t tss;  // Single TSS entry for task-switching back to kernel

// We don't use segmentation, so we provided a bare-minimum GDT.
void os3_setup_gdt() {
  gdt_entry_t *null_descriptor = &gdt[0], *code_descriptor = &gdt[1],
              *data_descriptor = &gdt[2], *user_code_descriptor = &gdt[3],
              *user_data_descriptor = &gdt[4], *tss_descriptor = &gdt[5];

  // Null descriptor, which cannot be used.
  uint32_t tss_base = (uint32_t)&tss;
  gdt_describe(null_descriptor, 0, 0, 0, 0);
  gdt_describe(code_descriptor, 0, LIMIT_4GB, 0x9a, 0xcf);
  gdt_describe(data_descriptor, 0, LIMIT_4GB, 0x92, 0xcf);
  gdt_describe(user_code_descriptor, 0, LIMIT_4GB, 0xfa, 0xcf);
  gdt_describe(user_data_descriptor, 0, LIMIT_4GB, 0xf2, 0xcf);
  gdt_describe(tss_descriptor, tss_base, tss_base + sizeof(os3_tss_entry_t),
               0xe9, 0);

  // Describe the actual GDT.
  gdt_descriptor.offset = (uint32_t)&gdt;
  gdt_descriptor.size = sizeof(gdt) - 1;

  // TSS Setup.
  tss.ss0 = 0x13;

  // Load it!
  asm("lgdt %0" ::"m"(gdt_descriptor));
  os3_flush_gdt((uint32_t)&gdt_descriptor);
}
