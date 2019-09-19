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

// PIC stuff
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

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

extern void os3_flush_idt(idt_descriptor_t *desc);

// TODO: Find a cleaner way to do this.
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// Syscall interrupt
extern void isr48();

#define INTERRUPT(n, ptr, handler, sel, ring)                                 \
  used_entries++;                                                             \
  (ptr).offset_0_15 = (((uint32_t)(handler)) & 0xffff);                     \
  (ptr).offset_16_31 = (((uint32_t)(handler)) >> 16) & 0xffff;              \
  (ptr).selector = (sel);                                                    \
  (ptr).always_zero = 0;                                                     \
  (ptr).type_attr = (INTERRUPT_GATE_32_BIT |                                 \
		      DESCRIPTOR_PRIVILEGE_RING##ring | DESCRIPTOR_PRESENT) | \
		     0x60;                                                    \
  (ptr).type_attr = 0x8e;
//
idt_entry_t idt_entries[256];
idt_descriptor_t idt_descriptor;
int used_entries = 0;

#define DECLARE_ISR(type) \
  INTERRUPT((type), idt_entries[(type)], (isr##type), 0x08, 0)
#define DECLARE_IRQ(type) \
  INTERRUPT((type) + 32, idt_entries[(type) + 32], (irq##type), 0x08, 0)

static void irq_enable(uint8_t no) {
  uint16_t pic = PIC1;
  if (no >= 8) {
    pic = PIC2;
    no -= 8;
  }
  uint8_t value = inb(pic + 1) | (1 << no);
  outb(pic + 1, value);
}

void os3_setup_idt() {
  // Before we load the IDT, remap the PIC.
  int pic1 = 0x20, pic2 = 0x28;

  // Initialize
  outb(PIC1, ICW1);
  outb(PIC2, ICW1);

  // Actually set the new offsets.
  outb(PIC1 + 1, pic1);
  outb(PIC2 + 1, pic2);

  // Make PIC1 aware of PIC2.
  outb(PIC1 + 1, 4);
  outb(PIC2 + 1, 2);

  // Send ICW4, then disable all IRQ's.
  outb(PIC1 + 1, ICW4);
  outb(PIC2 + 1, ICW4);
  outb(PIC1 + 1, 0xff);
  outb(PIC1 + 2, 0xff);

  // Next, enable the interrupts we have set up.
  for (int i = 0; i < used_entries; i++) {
    irq_enable(i);
  }

  // IDT time.
  kmemset(&idt_entries, 0, sizeof(idt_entries));
  idt_descriptor.size = sizeof(idt_entries) - 1;

  DECLARE_ISR(0);
  DECLARE_ISR(1);
  DECLARE_ISR(2);
  DECLARE_ISR(3);
  DECLARE_ISR(4);
  DECLARE_ISR(5);
  DECLARE_ISR(6);
  DECLARE_ISR(7);
  DECLARE_ISR(8);
  DECLARE_ISR(9);
  DECLARE_ISR(10);
  DECLARE_ISR(11);
  DECLARE_ISR(12);
  DECLARE_ISR(13);
  DECLARE_ISR(14);
  DECLARE_ISR(15);
  DECLARE_ISR(16);
  DECLARE_ISR(17);
  DECLARE_ISR(18);
  DECLARE_ISR(19);
  DECLARE_ISR(20);
  DECLARE_ISR(21);
  DECLARE_ISR(22);
  DECLARE_ISR(23);
  DECLARE_ISR(24);
  DECLARE_ISR(25);
  DECLARE_ISR(26);
  DECLARE_ISR(27);
  DECLARE_ISR(28);
  DECLARE_ISR(29);
  DECLARE_ISR(30);
  DECLARE_ISR(31);
  DECLARE_IRQ(0);
  DECLARE_IRQ(1);
  DECLARE_IRQ(2);
  DECLARE_IRQ(3);
  DECLARE_IRQ(4);
  DECLARE_IRQ(5);
  DECLARE_IRQ(6);
  DECLARE_IRQ(7);
  DECLARE_IRQ(8);
  DECLARE_IRQ(9);
  DECLARE_IRQ(10);
  DECLARE_IRQ(11);
  DECLARE_IRQ(12);
  DECLARE_IRQ(13);
  DECLARE_IRQ(14);
  DECLARE_IRQ(15);
  // This is CRITICAL: Align the pointer on 16 bits.
  // I spent several hours hunting this down.
  idt_descriptor.offset = (uint32_t)&idt_entries;

  // In hindsight, DO NOT "16-bit align" this.
  // It basically means you put the *wrong* address in
  // the IDTR, so everything will invariably break.
  // idt_descriptor.offset >>= 16;

  // Syscall interrupt = 48 = 0x30
  // DECLARE_ISR(48);

  // Now, load the IDT, and enable interrupts.
  asm volatile("lidt %0" ::"m"(idt_descriptor));
  // asm volatile("sti");
  // kwrites("IDT struct location: 0x");
  // kputi_r((uint32_t)&idt_descriptor, 16);
	// kputc('\n');
  // kwrites("IDT table[0] location: 0x");
  // kputi_r(idt_descriptor.offset, 16);
	// kputc('\n');
  // os3_flush_idt(&idt_descriptor);

  // TODO: Remove this
  // For now, only enable the keyboard.
  outb(PIC1 + 1, 0xfd);
  outb(PIC2 + 1, 0xff);
  asm volatile("sti");
}
