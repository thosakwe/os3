#ifndef OS3_PROCESS_H
#define OS3_PROCESS_H
#include <stdbool.h>
#include "file_descriptor.h"

typedef struct _os3_process {
	unsigned int id, page_table_index;
	int exit_code;
	bool is_kernel_module;
	struct _os3_process* next;
	os3_fd_t* file_descriptors;
} os3_process_t;

#endif
