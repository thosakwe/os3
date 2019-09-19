#include <os3_kernel.h>
#define ISR_GENERAL_PROTECTION_FAULT 0xd

void interrupt_handler(os3_interrupt_t *ctx) {
  // asm volatile("cli");
  if (ctx->number < 32) {
    switch (ctx->number) {
      case ISR_GENERAL_PROTECTION_FAULT: {
	// We hit a page fault.
	// No big deal, we can handle it.
	handle_general_protection_fault(ctx);
      } break;
      default: {
	kwrites("Unhandled ISR interrupt: ");
	kputi(ctx->number);
	kwrites("; error code=");
	kputi(ctx->error_code);
	kputc('\n');
      } break;
    }
  } else if (ctx->number >= 32 && ctx->number <= 48) {
    uint32_t irq_no = ctx->number - 32;
    switch (irq_no) {
      default: {
	kwrites("Unhandled IRQ interrupt: ");
	kputi(irq_no);
	kputc('\n');
      } break;
    }
    // Acknowledge the PIC; send EOI
    outb(0x20, 0x20);
    if (irq_no >= 8) outb(0xa0, 0x20);
  } else {
    kwrites("Got unsupported interrupt: ");
    kputi(ctx->number);
    kputc('\n');
  }
  // asm volatile("sti");
}

void handle_general_protection_fault(os3_interrupt_t *ctx) {
  // If the page requested was not present, then cr2 will
  // specify the faulty address.
  // void *ptr = 0;
  // asm("mov %%cr2, %0" : "=g"(ptr));
  kwrites("GPF on pointer: 0x");
  // kputi_r((unsigned int)ptr, 16);
  kputc('\n');

  // Try to allocate a page.
  // We can compute the address of the necessary page
  // easily.
  //
  // TODO: Create an allocation record like usual.
  // First, check if the page has been marked present.
  // Then, proceed accordingly, so that the page is not
  // entirely unusable, and so the memory in question
  // does not get overwritten.
  // size_t page_no = ((size_t)ptr) / PAGE_SIZE;
  // mark_page_present(page_no);
}
