#include <os3_kernel.h>

os3_process_t* os3_new_process(os3_t* os3) {
  os3_process_t* p = (os3_process_t*)kmalloc(sizeof(os3_process_t));
  if (p == NULL) return p;
  unsigned int id = -1;
  if (os3->processes == NULL) {
    os3->processes = p;
  } else {
    os3_process_t* cur = os3->processes;
    while (cur->next != NULL) {
      id = cur->id;
      cur = cur->next;
    }
    cur->next = p;
  }
  p->argc = 0;
  p->argv = NULL;
  p->entry_point = NULL;
  p->entry_point_size = 0;
  p->exit_code = -1;
  p->file_descriptors = NULL;
  p->id = id + 1;
  p->is_kernel_module = false;
  p->memory_start = NULL;
  p->page_table_index = 0;  // TODO: Page table index
	p->page_directory = NULL;
	p->page_tables = NULL;
  // p->page_directory =
  //     (uint32_t*)kmalloc(sizeof(uint32_t) * PAGE_DIRECTORY_SIZE);
  // p->page_tables =
  //     (os3_page_table_t*)kmalloc(sizeof(os3_page_table_t) * PAGE_TABLE_SIZE);
  p->stdin = os3_new_fd_std(p);
  p->stdout = os3_new_fd_std(p);
  p->stderr = os3_new_fd_std(p);
  return p;
}

