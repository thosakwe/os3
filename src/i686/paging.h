#ifndef OS3_I686_PAGING_H
#define OS3_I686_PAGING_H
#include <stdint.h>
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE PAGE_DIRECTORY_SIZE
#define PAGE_SIZE 4096
#define BYTES_IN_PAGE_TABLE (PAGE_TABLE_SIZE * PAGE_SIZE)
#define PAGE_MASK_EMPTY 0xfffff000
#define PAGE_MASK_PRESENT 0x1
#define PAGE_MASK_READ_WRITE 0x2
#define PAGE_MASK_RING0 0x0
#define PAGE_MASK_RING3 0x4
#define SIZE_1MB (1024 * 1024)
#define SIZE_1GB (SIZE_1MB * 1024)
#define SIZE_4GB ((uint64_t)SIZE_1GB * 4)

typedef struct _os3_page_table {
  uint32_t pages[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));
} __attribute__((packed)) os3_page_table_t;

extern uint32_t page_directory[PAGE_DIRECTORY_SIZE]
    __attribute__((aligned(4096)));
extern os3_page_table_t page_tables[PAGE_DIRECTORY_SIZE]
    __attribute__((aligned(4096)));

extern void os3_enable_paging(uint32_t *pdir);
int16_t next_page_directory();
void release_page_directory(int16_t);

#endif
