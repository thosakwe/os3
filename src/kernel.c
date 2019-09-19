#include <os3_kernel.h>
 
void kernel_main(void) 
{
	os3_t os3;
	os3.processes = NULL;

	os3_setup_gdt();
  kwrites("milestone");
	// os3_setup_idt();
	// os3_setup_paging();

	// asm("int $0x2");
	// kwrites("a");
	// asm("int $0x5");
	// asm("int $0x0d");
	// asm("int $0x80");
	// kwrites("b");
}
