#ifndef OS3_I686_MEMORY_H
#define OS3_I686_MEMORY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
	OS3_MEMORY_READABLE,
	OS3_MEMORY_WRITABLE,
	OS3_MEMORY_EXECUTABLE
} os3_memory_access_t;

typedef struct _os3_memory_region {
  bool used;
  size_t offset, length;
	os3_memory_access_t access;
  struct _os3_memory_region *next, *prev;
} os3_memory_region_t;

// TODO: There's not really a need for this to be static/inline
static inline void os3_memory_region_init(os3_memory_region_t* p) {
	p->used = false;
	p->access = OS3_MEMORY_READABLE;
	p->next = p->prev = NULL;
	p->offset = p->length = 0;
}

#endif
