#include <os3_kernel.h>

void kmemset(void *ptr, uint8_t value, size_t size) {
  uint8_t *p = (uint8_t *)ptr;
  for (size_t i = 0; i < size; i++) {
    p[i] = value;
  }
}

void kmemcpy(void *dst, void *src, unsigned long size) {
  uint8_t *d = (uint8_t *)dst;
  uint8_t *s = (uint8_t *)src;
  for (unsigned long i = 0; i < size; i++) {
    d[i] = s[i];
  }
}
