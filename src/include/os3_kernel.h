#ifndef OS3_KERNEL_H
#define OS3_KERNEL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "liballoc_1_1.h"
#include "process.h"
#define COM1 0x3F8

typedef struct {
  uint32_t cr2;
  uint32_t gs;
  uint32_t fs;
  uint32_t ds;
  uint32_t es;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t number; // The interrupt number, i.e. 0x80
  uint32_t error_code;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp;
  uint32_t ss;
} __attribute__((packed)) os3_interrupt_t;

void interrupt_handler(os3_interrupt_t *ctx);
void handle_general_protection_fault(os3_interrupt_t *ctx);

typedef struct _os3 {
  os3_process_t* processes;
} os3_t;

extern uint32_t endkernel;
void os3_setup_gdt();
void os3_setup_idt();
void os3_setup_paging(uint32_t ram_start, uint32_t ram_end);
os3_process_t* os3_new_process(os3_t* os3);
int os3_enter_process(os3_t* os3, os3_process_t* proc);
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

void kmemset(void* ptr, uint8_t value, size_t size);
void kmemcpy(void *dst, void *src, unsigned long size);

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

static inline void interrupt(uint8_t no) { asm volatile("int $%0" ::"Nd"(no)); }

#endif
