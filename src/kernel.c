#include <os3_kernel.h>
 
void kernel_main(void) 
{
  kputs("Hello, /u/thosakwe!");
	os3_setup_gdt();
	os3_setup_idt();
	// os3_setup_paging();
}
