#include <os3_kernel.h>

typedef struct {
  uint16_t size;
  uint32_t offset;
} __attribute__((packed)) gdt_descriptor_t;

typedef struct {
  uint16_t limit_0_15;
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access;
  uint8_t limit_16_19_and_flags;
  uint8_t base_24_31;
} __attribute__((packed)) gdt_entry_t;

#define LIMIT_4GB 0xffffffff

gdt_descriptor_t gdt_descriptor;
gdt_entry_t gdt[5];

/** Fills a GDT entry. */
static inline void gdt_describe(gdt_entry_t *entry, uint32_t base,
                                uint32_t limit, uint8_t access, uint8_t flags) {
  // Set the base values
  entry->base_0_15 = base & 0xffff;
  entry->base_16_23 = (base >> 16) & 0xff;
  entry->base_24_31 = (base >> 24) & 0xff;

  // Set the access
  entry->access = access;

  // Set the limit
  entry->limit_0_15 = limit & 0xffff;

  // Set the flags
  entry->limit_16_19_and_flags = (limit >> 16) & 0x0f;
  entry->limit_16_19_and_flags |= (flags & 0xf0);
}

// We don't use segmentation, so we provided a bare-minimum GDT.
void os3_setup_gdt() {
  gdt_entry_t *null_descriptor = &gdt[0], *code_descriptor = &gdt[1],
              *data_descriptor = &gdt[2], *user_code_descriptor = &gdt[3],
              *user_data_descriptor = &gdt[4];

  // Null descriptor, which cannot be used.
  gdt_describe(null_descriptor, 0, 0, 0, 0);
  gdt_describe(code_descriptor, 0, LIMIT_4GB, 0x9a, 0xcf);
  gdt_describe(data_descriptor, 0, LIMIT_4GB, 0x92, 0xcf);
  gdt_describe(user_code_descriptor, 0, LIMIT_4GB, 0xfa, 0xcf);
  gdt_describe(user_data_descriptor, 0, LIMIT_4GB, 0xf2, 0xcf);

  // Describe the actual GDT.
  gdt_descriptor.offset = (uint32_t)&gdt;
  gdt_descriptor.size = sizeof(gdt) - 1;

  // Load it!
  asm("lgdt %0" ::"m"(gdt_descriptor));
}
