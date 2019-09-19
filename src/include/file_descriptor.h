#ifndef OS_FILE_DESCRIPTOR_H
#define OS_FILE_DESCRIPTOR_H
#include <stdint.h>

// OS/3 does not have a built-in filesystem, so file descriptors
// are actually either:
//   * Standard I/O streams
//   * IPC channels to other processes

typedef enum {
  OS3_FD_STDIN,
  OS3_FD_STDOUT,
  OS3_FD_STDERR,
  OS3_FD_IPC
} os3_fd_type_t;

typedef struct _os3_blob {
  unsigned int size;
  uint8_t* data;
  struct _os3_blob* next;
} os3_blob_t;

typedef struct _os3_fd {
  unsigned int id;
  os3_fd_type_t type;
  os3_blob_t *incoming, *outgoing;
  struct _os3_fd* next;
} os3_fd_t;

#endif
