#include <os3_kernel.h>

/* void kput_at(char ch, int y, int x) {} */

void kcls() {}

void kputc(char ch) { outb(COM1, ch); }

void kputs(const char *text) {
  kwrites(text);
  kputc('\n');
}

void kwrites(const char *text) {
  uint16_t len = kshortstrlen(text);
  for (uint16_t i = 0; i < len; i++) {
    kputc(text[i]);
  }
}

void kputi(int val) { kputi_r(val, 10); }

void kputi_r(int val, int base) {
  char buf[33] = {0};
  kitoa(val, buf, base);
  kwrites(buf);
}

uint16_t kshortstrlen(const char *text) {
  const char *ptr = text;
  uint16_t len = 0;

  while (*ptr != 0) {
    ptr++;
    len++;
  }

  return len;
}

// From https://wiki.osdev.org/Printing_To_Screen
char *kitoa(int value, char *str, int base) {
  char *rc;
  char *ptr;
  char *low;
  // Check for supported base.
  if (base < 2 || base > 36) {
    *str = '\0';
    return str;
  }
  rc = ptr = str;
  // Set '-' for negative decimals.
  if (value < 0 && base == 10) {
    *ptr++ = '-';
  }
  // Remember where the numbers start.
  low = ptr;
  // The actual conversion.
  do {
    // Modulo is negative for negative value. This trick makes abs()
    // unnecessary.
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrst"
             "uvwxyz"[35 + value % base];
    value /= base;
  } while (value);
  // Terminating the string.
  *ptr-- = '\0';
  // Invert the numbers.
  while (low < ptr) {
    char tmp = *low;
    *low++ = *ptr;
    *ptr-- = tmp;
  }
  return rc;
}
