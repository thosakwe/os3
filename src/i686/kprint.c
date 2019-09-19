#include <os3_kernel.h>

/* void kput_at(char ch, int y, int x) {} */

void kcls() {}

void kputc_old(char ch) { outb(COM1, ch); }

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
  char buf[33];  // = {0};
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
    *ptr++ =
	"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrst"
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

// Now that we're in Bochs, write to the screen...

#define COLUMNS 80
#define ROWS 25

typedef struct {
  uint8_t ascii_code;
  uint8_t attr;
} __attribute__((packed)) x86_char_t;

// Struct to hold each line.
typedef struct {
  uint8_t length;
  uint8_t text[COLUMNS];
} term_line_t;

x86_char_t *video_memory = (x86_char_t *)0xb8000;
term_line_t lines[ROWS];
uint8_t term_init = 0;
uint8_t line_no = 0;

void kterm_init() {
  if (term_init == 0) {
    for (int y = 0; y < ROWS; y++) {
      lines[y].length = 0;
      kmemset(&lines[y].text, 0, COLUMNS);
      for (int x = 0; x < COLUMNS; x++) {
	int index = y * COLUMNS + x;
	video_memory[index].ascii_code = ' ';
	video_memory[index].attr = 0;
      }
    }
    term_init = 1;
  }
}

void kputc(char ch) {
  // Also write to serial, because the screen is only so tall.
  // outb(COM1, ch);
  kterm_init();
  if (ch != '\n') {
    term_line_t *line = &lines[line_no];
    line->text[line->length] = ch;
    int index = line_no * COLUMNS + line->length;
    video_memory[index].ascii_code = ch;
    video_memory[index].attr = 7;
    if ((++line->length) >= COLUMNS) {
      kputc('\n');
    }
  } else {
    // If we haven't hit the bottom yet, just
    // advance a line.
    if (++line_no >= ROWS) {
      // To scroll, we move each saved line's text up one row.
      // We also print everything.
      for (int i = 1; i < ROWS; i++) {
	term_line_t *old_line = &lines[i - 1];
	term_line_t *new_line = &lines[i];
	// First, write the new line's text.
	for (int j = 0; j < new_line->length; j++) {
	  int index = (i - 1) * COLUMNS + j;
	  video_memory[index].ascii_code = new_line->text[j];
	  video_memory[index].attr = 7;
	}
	// Then, write blanks where necessary.
	for (int j = new_line->length; j < old_line->length; j++) {
	  int index = (i - 1) * COLUMNS + j;
	  video_memory[index].ascii_code = ' ';
	  video_memory[index].attr = 0;
	}
	// Now that printing is done, overwrite the old entry.
	kmemcpy(old_line->text, new_line->text, new_line->length);
	// Turns out this memset is unnecessary, AND causes a hang.
	// kmemset(&old_line->text[new_line->length], 0,
	//         old_line->length - new_line->length);
	old_line->length = new_line->length;
      }

      // If this is the last line, then draw blanks entirely.
      term_line_t *last = &lines[ROWS - 1];
      last->length = 0;
      for (int j = 0; j < COLUMNS; j++) {
	int index = (ROWS - 1) * COLUMNS + j;
	video_memory[index].ascii_code = ' ';
	video_memory[index].attr = 0;
      }

      // We then reset line_no to ROWS, as from now on, we will
      // always need to scroll (until we clear the screen).
      line_no = ROWS - 1;
    }
  }
}
