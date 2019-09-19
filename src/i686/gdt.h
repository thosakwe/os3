#ifndef OS3_I686_GDT_H
#define OS3_I686_GDT_H
#include <stdint.h>

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

// extern gdt_entry_t* gdt;
extern void os3_flush_gdt(uint32_t ptr);

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

#endif
