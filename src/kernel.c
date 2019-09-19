#include <os3_kernel.h>
 
void kernel_main(void) 
{
	os3_setup_gdt();
	os3_setup_idt();
	os3_setup_paging();

	int sz = 1024;
	int* ints = kmalloc(sizeof(int) * sz);
	for (int i = 0; i < sz; i++) {
		ints[i] = sz - i;
	}

	for (int i = 0; i < sz; i++) {
		kputi(ints[i]);
		kputc('\n');
	}
}
