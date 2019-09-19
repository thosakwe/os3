#include <os3_kernel.h>

os3_fd_t* os3_new_fd_std(os3_process_t* process) {
	os3_fd_t* p = (os3_fd_t*)kmalloc(sizeof(os3_fd_t));
	if (p == NULL) return p;
	unsigned int id = -1;
	if (process->file_descriptors == NULL) {
		process->file_descriptors = p;
	} else {
		os3_fd_t* cur = process->file_descriptors;
		while (cur->next != NULL) {
			id = cur->id;
			cur = cur->next;
		}
		cur->next = p;
	}
	p->id = id + 1;
	p->is_closed = false;
	p->incoming = p->outgoing = NULL;
	p->next = NULL;
	p->type = OS3_FD_STD_STREAM;
	return p;
}
