#ifndef OS_FILE_DESCRIPTOR_H
#define OS_FILE_DESCRIPTOR_H

typedef struct _os3_fd {
	unsigned int id;
	struct _os3_fd* next;
} os3_fd_t;

#endif
