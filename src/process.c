#include <os3_kernel.h>

os3_process_t* os3_new_process(os3_t* os3) {
  os3_process_t* p = (os3_process_t*)kmalloc(sizeof(os3_t));
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
  p->exit_code = -1;
  p->file_descriptors = NULL;
  p->id = id + 1;
  p->is_kernel_module = false;
  p->page_table_index = 0;  // TODO: Page table index
  p->stdin = os3_new_fd_std(p);
  p->stdout = os3_new_fd_std(p);
  p->stderr = os3_new_fd_std(p);
  return p;
}

