#include <os3_kernel.h>
#define IRQ_INVALID_TSS 0xa
#define ISR_GENERAL_PROTECTION_FAULT 0xd
#define ISR_PAGE_FAULT 0xe

void os3_sysenter_entry() {
  kputs("SYSENTER WORKED");
  while(true) {
    continue;
  }
}

uint8_t interrupt_handler(os3_interrupt_t *ctx) {
  // asm volatile("cli");
  // Acknowledge the PIC; send EOI
  outb(0x20, 0x20);
  if (ctx->number < 32) {
    switch (ctx->number) {
    case ISR_GENERAL_PROTECTION_FAULT: {
      // We hit a GPF fault.
      // No big deal, we can handle it.
      handle_general_protection_fault(ctx);
    } break;
    case ISR_PAGE_FAULT: {
      handle_page_fault(ctx);
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
    case IRQ_INVALID_TSS: {
      uint32_t selector_index = ctx->error_code;
      kwrites("Invalid TSS. Selector index: 0x");
      kputi_r(selector_index, 16);
      kputc('\n');
    } break;
    default: {
      kwrites("Unhandled IRQ interrupt: ");
      kputi(irq_no);
      kputc('\n');
    } break;
    }
    // Acknowledge the PIC; send EOI
    if (irq_no >= 8)
      outb(0xa0, 0x20);
  } else if (ctx->number == 128) {
    kputs("GOT A SYSCALL!!!!");
    kputptr("eax", ctx->eax);
    if (ctx->eax == 0) {
      // Don't iret.
      return 1;
    }
  } else {
    kwrites("Got unsupported interrupt: 0x");
    kputi_r(ctx->number, 16);
    kputc('\n');
  }
  // asm volatile("sti");
  return 0;
}

void handle_general_protection_fault(os3_interrupt_t *ctx) {
  // If the page requested was not present, then cr2 will
  // specify the faulty address.
  void *ptr = get_page_fault_pointer();
  kputptr("GPF on pointer", ptr);
  hang();
}

void handle_page_fault(os3_interrupt_t *ctx) {
  uint8_t info = ctx->error_code;
  void *ptr = get_page_fault_pointer();
  uint32_t page_index = ((uint32_t)ptr) / PAGE_SIZE;
	kputptr("Page fault on pointer", ptr);
	kwrites("Page index: 0x");
	kputi_r(page_index, 16);
	kputc('\n');
  if (info == 0x0) {
    kputs("Attempt to read non-present page.");
  } else if (info == 0x2) {
    kputs("Attempt to write to non-present page.");
  } else if (info == 0x4) {
    kputs("Attempt to read non-present page in ring0.");
  } else if (info == 0x5) {
    kputs("Attempt to read ring0 data from ring3.");
  } else if (info == 0x6) {
    kputs("Attempt to write ring0 data from ring3.");
  } else {
    kwrites("Info: 0b");
    kputi_r(info, 2);
    kputc('\n');
  }
  hang();
  // Try to allocate a spare page in a new directory.
  // TODO: Release this page dir later?
  // int index = next_page_directory();
  // if (index == -1) {
  // 	kputs("Ran out of memory while trying to rectify page fault.");
  // 	return;
  // }

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
