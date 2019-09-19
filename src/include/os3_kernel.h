#ifndef OS3_KERNEL_H
#define OS3_KERNEL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "liballoc_1_1.h"
#include "process.h"
#define COM1 0x3F8

typedef struct _os3 {
  os3_process_t* processes;
} os3_t;

void os3_setup_gdt();
void os3_setup_idt();
void os3_setup_paging();
os3_process_t* os3_new_process(os3_t* os3);
void os3_destroy_process(os3_t* os3, os3_process_t process);
os3_fd_t* os3_new_fd_std(os3_process_t* process);
os3_fd_t* os3_new_fd_ipc(os3_process_t* process, unsigned int foreign_pid);
void os3_destroy_fd(os3_process_t process, os3_fd_type_t* fd);

/** Convert an integer to a string. */
char* kitoa(int value, char* str, int base);

/** Clears the screen.*/
void kcls();

/** Writes a single character to the screen. */
void kputc(char ch);

/** Writes a string to the screen. */
void kwrites(const char* text);

/** Writes a string to the screen, followed by a newline. */
void kputs(const char* text);

/** Writes an integer to the screen as base-10. */
void kputi(int val);

/** Writes an integer to the screen. */
void kputi_r(int val, int base);

/**
 * Finds the length of a string that is KNOWN to be <= 65535 characters long.
 *  Should work in almost every case, honestly.
 */
uint16_t kshortstrlen(const char* text);

void* kmalloc(size_t size);
void kfree(void* ptr);

/** Sends a value through a serial port. */
static inline void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

/** Receives a value from a serial port. */
static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void interrupt(uint8_t no) { asm volatile("int %0" ::"Nd"(no)); }

#endif
