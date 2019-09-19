#include <os3_kernel.h>
 
void kernel_main(void) 
{
	os3_setup_gdt();
	os3_setup_idt();
	os3_setup_paging();
}
