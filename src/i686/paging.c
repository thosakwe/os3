#include "tss.h"
#include <multiboot2.h>
#include <os3_kernel.h>

uint32_t page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096)));
os3_page_table_t page_tables[PAGE_DIRECTORY_SIZE]
    __attribute__((aligned(4096)));

// bool used_dirs[PAGE_DIRECTORY_SIZE];
uint64_t _ram_end;
uint32_t id_map_page_count;

// int16_t next_page_directory() {
//   for (int16_t i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
//     if (!page_is_present(page_directory[i])) {
//       // if (!(used_dirs[i])) {
//       // used_dirs[i] = true;
//       page_directory[i] |= PAGE_MASK_PRESENT;
//       return i;
//     }
//   }
//   return -1;
// }

// void release_page_directory(int16_t n) {
//   // TODO: Mark page directory as unused.
//   page_directory[n] &= ~PAGE_MASK_PRESENT;
//   // used_dirs[n] = false;
// }

// void os3_enable_kernel_pages() {
//   for (uint32_t i = 0; i < id_map_page_count; i++) {
//     page_directory[i] |= PAGE_MASK_PRESENT;
//   }
// }
// void os3_disable_kernel_pages() {
//   for (uint32_t i = 0; i < id_map_page_count; i++) {
//     page_directory[i] &= (~PAGE_MASK_PRESENT);
//   }
// }

void os3_init_page_directory(uint32_t *pd, os3_page_table_t *pt, uint32_t rs) {
  // First, set up our page directory. Every entry should point to
  // a page table.
  for (uint16_t i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
    os3_page_table_t *table = &pt[i];
    uint32_t table_addr = (uint32_t)table;
    pd[i] = table_addr & PAGE_MASK_EMPTY;
    // if (ud != NULL) ud[i] = true;

    // While we loop, also initialize each page table.
    for (uint16_t j = 0; j < PAGE_TABLE_SIZE; j++) {
      // Each page points to a 4KB offset from ram_start.
      multiboot_uint64_t addr = rs + (PAGE_SIZE * j);
      // uint32_t addr = ram_start + (PAGE_SIZE * j);
      table->pages[j] = addr & PAGE_MASK_EMPTY;
    }
  }
}

void os3_setup_paging(uint32_t ram_start, uint32_t ram_end) {
  // Init used_pages.
  // for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
  //   used_dirs[i] = false;
  // }
  // for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
  //   used_pages[i] = false;
  // }

  // First, set up our page directory. Every entry should point to
  // a page table.
  os3_init_page_directory(page_directory, page_tables, ram_start);

  // Next, identity map the kernel.
  // Simply put: We simply set (present) on pages in the first
  // page table, until we have passed the end of the kernel.
  //
  // We tell the MMU that the region 0x0 - 0x(endkernel)
  // is 1:1 mapped to itself.
  // uint64_t end = (uint64_t)&endkernel;
  uint64_t end = ram_end;
  // end = (uint64_t)endkernel;
  _ram_end = end;
  uint64_t offset = 0;
  uint32_t pde_index = 0;
  uint64_t identity_map_count = end;
  uint64_t endk = (uint64_t)&endkernel;
  end = _ram_end = endk;
  while (offset < identity_map_count && pde_index < PAGE_DIRECTORY_SIZE) {
    // Should everything be ring3-able?
    page_directory[pde_index] |=
        PAGE_MASK_RING3 | PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE;

    // Though we are marking everything present, don't mark any as USED if it's
    // not actually used. Otherwise, there will be no successful kmalloc calls,
    // ever.
    // if (offset <= endk) {
    //   used_dirs[pde_index] = true;
    // }

    for (int i = 0; i < PAGE_TABLE_SIZE && offset < identity_map_count; i++) {
      // used_pages[i] = true;
      // kputptr("Offset", (void *)offset);
      // kputptr("PDE index", (void *)pde_index);
      // kputptr("Page table index", i);
      uint32_t value = page_tables[pde_index].pages[i] =
          offset | PAGE_MASK_PRESENT | PAGE_MASK_READ_WRITE | PAGE_MASK_RING3;
      offset += PAGE_SIZE;
    }
    pde_index++;
  }
  kputptr("Final offset", (void *)offset);
  kputptr("Endkernel", &endkernel);

  id_map_page_count = pde_index;

  // Next, we need to make the usermode_exit function user-accessible.
  // TODO: Only make usermode_exit ring3, not the entire kernel.

  // Next, load the page directory, and enable paging.
  // kwrites("Page directory offset: 0x");
  // kputi_r((unsigned long)&page_directory, 16);
  // kwrites("Page table 0 list offset: 0x");
  // kputi_r((unsigned long)&page_tables[0], 16);
  os3_enable_paging(page_directory);
}

void os3_page_set_flags(void *ptr, size_t n_pages, uint32_t flags) {
  uint16_t i = ((uint32_t)ptr) / PAGE_SIZE;
  for (uint16_t j = 0; j < i + n_pages; j++) {
    uint16_t page_index = i + j;
    uint16_t pde_index = page_index / PAGE_DIRECTORY_SIZE;
    uint16_t pt_index = page_index % PAGE_DIRECTORY_SIZE;
    // Mark the directory present.
    page_directory[pde_index] |= PAGE_MASK_PRESENT;
    // Mark the page present.
    page_tables[pde_index].pages[pt_index] |= PAGE_MASK_PRESENT;
  }
}

int liballoc_lock() { return 0; }

int liballoc_unlock() { return 0; }

void *liballoc_alloc(size_t n) { return os3_get_pages(n, PAGE_MASK_RING0); }

void *os3_get_pages(size_t n, uint32_t flags) {
  // TODO: This is potentially very slow.
  // Skip the ID-mapped region.
  uint16_t min_page = (_ram_end / PAGE_SIZE) + 1;
  for (uint16_t i = min_page; i < TOTAL_PAGE_COUNT - n; i++) {
    bool matched = true;
    for (size_t j = 0; (j < n) && matched; j++) {
      uint16_t page_index = i + j;
      uint16_t pde_index = page_index / PAGE_DIRECTORY_SIZE;
      uint16_t pt_index = page_index % PAGE_DIRECTORY_SIZE;
      // If the directory is not present, then the page is not present.
      if (!page_is_present(page_directory[pde_index])) {
        continue;
      }
      // Otherwise, check if the page is present.
      uint32_t page = page_tables[pde_index].pages[pt_index];
      if ((page & PAGE_MASK_PRESENT) == PAGE_MASK_PRESENT) {
        matched = false;
        break;
      }
    }
    if (matched) {
      // Set all dirs and entries present.
      for (size_t j = 0; (j < n) && matched; j++) {
        uint16_t page_index = i + j;
        uint16_t pde_index = page_index / PAGE_DIRECTORY_SIZE;
        uint16_t pt_index = page_index % PAGE_DIRECTORY_SIZE;
        // Mark the directory present.
        page_directory[pde_index] |= PAGE_MASK_PRESENT | flags;
        // Mark the page present.
        page_tables[pde_index].pages[pt_index] |= PAGE_MASK_PRESENT | flags;
      }
      uint16_t page_index = i;
      uint16_t pde_index = page_index / PAGE_DIRECTORY_SIZE;
      uint16_t pt_index = page_index % PAGE_DIRECTORY_SIZE;
      // kputptr("PDEIndex", pde_index);
      // kputptr("PTIndex", pt_index);
      // uint64_t offset = PAGE_DIRECTORY_SIZE * PAGE_SIZE;
      // for (uint16_t p = 0; p < pt_index; p++) {
      //   offset += PAGE_SIZE;
      // }
      uint64_t offset =
          (pde_index * PAGE_DIRECTORY_SIZE * PAGE_TABLE_SIZE * PAGE_SIZE);

      // return (void *)offset;
      return (void *)(i * PAGE_SIZE);
    }
  }
  return NULL;
}

int liballoc_free(void *ptr, size_t n) {
  int i = ((unsigned int)ptr) / PAGE_SIZE;
  for (int j = i; j < i + n; j++) {
    // Mark pages as NOT present.
    page_directory[j] &= PAGE_MASK_NOT_PRESENT;
    // used_pages[j] = false;
  }
  return 0;
}
