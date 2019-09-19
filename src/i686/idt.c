#include <os3_kernel.h>

// Gate types
#define TRAP_GATE_32_BIT 0xf
#define INTERRUPT_GATE_32_BIT 0xe
#define TRAP_GATE_16_BIT 0x7
#define INTERRUPT_GATE_16_BIT 0x6
#define TASK_GATE_32_BIT 0x5

// Other IDT flags
#define STORAGE_TASK_GATE 0b10000
#define DESCRIPTOR_PRIVILEGE_RING0 0b00000000
#define DESCRIPTOR_PRIVILEGE_RING1 0b00100000
#define DESCRIPTOR_PRIVILEGE_RING2 0b01000000
#define DESCRIPTOR_PRIVILEGE_RING3 0b01100000
#define DESCRIPTOR_PRESENT 0b10000000

typedef struct {
  uint16_t offset_0_15;
  uint16_t selector;
  uint8_t always_zero;
  uint8_t type_attr;
  uint16_t offset_16_31;
} __attribute__((packed)) idt_entry_t;

typedef struct {
  uint16_t size;
  uint32_t offset;
} __attribute__((packed)) idt_descriptor_t;

void interrupt_handler(uint8_t type);

#define IRQ_STUB(x)                                                            \
  void irq##x(void) {                                                          \
    outb(0x20, 0x20);                                                          \
    if (x >= 8)                                                                \
      outb(0xA0, 0x20);                                                        \
    interrupt_handler((x));                                                    \
  }

#define INTERRUPT(n, sel, ring)                                                \
  ((idt_entry_t){.offset_0_15 = (((uint32_t)&irq##n) & 0xffff),                \
                 .offset_16_31 = (((uint32_t)&irq##n) >> 16) & 0xffff,         \
                 .selector = (sel),                                            \
                 .always_zero = 0,                                             \
                 .type_attr =                                                  \
                     (INTERRUPT_GATE_32_BIT |                                  \
                      DESCRIPTOR_PRIVILEGE_RING##ring | DESCRIPTOR_PRESENT) |  \
                     0x60})

#define INTERRUPT_OLD(n, sel, ring)                                            \
  ((idt_entry_t){.offset_0_15 = ((uint32_t)&irq##n),                           \
                 .offset_16_31 = (((uint32_t)&irq##n) >> 16),                  \
                 .selector = (sel),                                            \
                 .always_zero = 0,                                             \
                 .type_attr =                                                  \
                     (INTERRUPT_GATE_32_BIT |                                  \
                      DESCRIPTOR_PRIVILEGE_RING##ring | DESCRIPTOR_PRESENT)})

#pragma region IRQ_STUBS
IRQ_STUB(0)
IRQ_STUB(1)
IRQ_STUB(2)
IRQ_STUB(3)
IRQ_STUB(4)
IRQ_STUB(5)
IRQ_STUB(6)
IRQ_STUB(7)
IRQ_STUB(8)
IRQ_STUB(9)
IRQ_STUB(10)
IRQ_STUB(11)
IRQ_STUB(12)
IRQ_STUB(13)
IRQ_STUB(14)
IRQ_STUB(15)
IRQ_STUB(16)
IRQ_STUB(17)
IRQ_STUB(18)
IRQ_STUB(19)
IRQ_STUB(20)
IRQ_STUB(21)
IRQ_STUB(22)
IRQ_STUB(23)
IRQ_STUB(24)
IRQ_STUB(25)
IRQ_STUB(26)
IRQ_STUB(27)
IRQ_STUB(28)
IRQ_STUB(29)
IRQ_STUB(30)
IRQ_STUB(31)
IRQ_STUB(32)
IRQ_STUB(33)
#pragma endregion

idt_entry_t idt_entries[34];
idt_descriptor_t idt_descriptor;

#define DECLARE_INTERRUPT(type) idt_entries[(type)] = INTERRUPT(type, 0x8, 0)

void os3_setup_idt() {
  idt_descriptor.size = sizeof(idt_entries) - 1;
  idt_descriptor.offset = (uint32_t)&idt_entries;
  DECLARE_INTERRUPT(0);
  DECLARE_INTERRUPT(1);
  DECLARE_INTERRUPT(2);
  DECLARE_INTERRUPT(3);
  DECLARE_INTERRUPT(4);
  DECLARE_INTERRUPT(5);
  DECLARE_INTERRUPT(6);
  DECLARE_INTERRUPT(7);
  DECLARE_INTERRUPT(8);
  DECLARE_INTERRUPT(9);
  DECLARE_INTERRUPT(10);
  DECLARE_INTERRUPT(11);
  DECLARE_INTERRUPT(12);
  DECLARE_INTERRUPT(13);
  DECLARE_INTERRUPT(14);
  DECLARE_INTERRUPT(15);
  DECLARE_INTERRUPT(16);
  DECLARE_INTERRUPT(17);
  DECLARE_INTERRUPT(18);
  DECLARE_INTERRUPT(19);
  DECLARE_INTERRUPT(20);
  DECLARE_INTERRUPT(21);
  DECLARE_INTERRUPT(22);
  DECLARE_INTERRUPT(23);
  DECLARE_INTERRUPT(24);
  DECLARE_INTERRUPT(25);
  DECLARE_INTERRUPT(26);
  DECLARE_INTERRUPT(27);
  DECLARE_INTERRUPT(28);
  DECLARE_INTERRUPT(29);
  DECLARE_INTERRUPT(30);
  DECLARE_INTERRUPT(31);
  DECLARE_INTERRUPT(32);
  DECLARE_INTERRUPT(33);
  asm("lidt %0" ::"m"(idt_descriptor));
}
