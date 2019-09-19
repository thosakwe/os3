#include <os3_kernel.h>
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

uint32_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096)));
os3_page_table_t page_tables[PAGE_DIRECTORY_SIZE]
    __attribute__((aligned(4096)));

extern void os3_enable_paging(uint32_t *pdir);
extern uint32_t endkernel;

bool used_pages[PAGE_TABLE_SIZE];

void os3_setup_paging() {
  // Init used_pages.
  for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
    used_pages[i] = false;
  }

  // First, set up our page directory. Every entry should point to
  // a page table.
  uint32_t ram_start = 0;
  for (uint16_t i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
    os3_page_table_t *table = &page_tables[i];
    uint32_t table_addr = (uint32_t)table;
    page_directory[i] = (table_addr)&PAGE_MASK_EMPTY;

    // While we loop, also initialize each page table.
    for (uint16_t j = 0; j < PAGE_TABLE_SIZE; j++) {
      // Each page points to a 4KB offset from ram_start.
      // multiboot_uint64_t addr = ram_start + (PAGE_SIZE * j);
      uint32_t addr = ram_start + (PAGE_SIZE * j);
      table->pages[0] = (addr)&PAGE_MASK_EMPTY;
    }
  }

  // Next, identity map the kernel.
  // Simply put: We simply set (present) on pages in the first
  // page table, until we have passed the end of the kernel.
  //
  // We tell the MMU that the region 0x0 - 0x(endkernel)
  // is 1:1 mapped to itself.
  uint64_t end = (uint64_t)&endkernel;
  uint64_t offset = 0;
  uint32_t pde_index = 0;
  uint64_t identity_map_count = end;
  while (offset < identity_map_count) {
    page_directory[pde_index] |= PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE;

    for (int i = 0; i < PAGE_DIRECTORY_SIZE && offset < identity_map_count;
	 i++) {
      used_pages[i] = true;
      uint32_t value = page_tables[pde_index].pages[i] =
	  offset | PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE;
      offset += PAGE_SIZE;
    }
    pde_index++;
  }

  // Next, load the page directory, and enable paging.
  // kwrites("Page directory offset: 0x");
  // kputi_r((unsigned long)&page_directory, 16);
  // kwrites("Page table 0 list offset: 0x");
  // kputi_r((unsigned long)&page_tables[0], 16);
  os3_enable_paging(page_directory);
}

int liballoc_lock() { return 0; }

int liballoc_unlock() { return 0; }

void *liballoc_alloc(size_t n) {
	const int num_pages = (sizeof(used_pages) / sizeof(bool));
	const int min_page = (((unsigned int)&endkernel) / PAGE_SIZE) + 1;
  // TODO: This can be made faster, but do I really care atm
  for (int i = min_page; i < num_pages - n; i++) {
		bool matched = true;
		for (int j = i; j < i + n; j++) {
			if (used_pages[j]) {
				matched = false;
				break;
			}
		}
		if (matched) {
			for (int j = i; j < i + n; j++) {
				// Mark pages as present.
				page_directory[j] |= PAGE_MASK_PRESENT;
				used_pages[j] = true;
			}
			return (void*)(i * PAGE_SIZE);
		}
  }
	return NULL;
}

int liballoc_free(void *ptr, size_t n) {
	int i = ((unsigned int)ptr) / PAGE_SIZE;
	for (int j = i; j < i + n; j++) {
		// Mark pages as NOT present.
		const int MASK_NOT_PRESENT = ~PAGE_MASK_PRESENT;
		page_directory[j] &= MASK_NOT_PRESENT;
		used_pages[j] = false;
	}
	return 0;
}
