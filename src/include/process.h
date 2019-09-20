#ifndef OS3_PROCESS_H
#define OS3_PROCESS_H
#include <stdbool.h>
#include "file_descriptor.h"
#include "memory.h"
#include "paging.h"

typedef struct _os3_process {
  unsigned int id, page_table_index;
  int exit_code;
  bool is_kernel_module;
  struct _os3_process *next;
  os3_fd_t *file_descriptors, *stdin, *stdout, *stderr;
  void *entry_point;
  size_t entry_point_size;
  os3_memory_region_t *memory_start;
	uint32_t *page_directory;
	os3_page_table_t* page_tables;
  int argc;
  char **argv;
} os3_process_t;

#endif
