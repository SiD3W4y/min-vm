#ifndef MINFILE_H
#define MINFILE_H

#define MINFILE_SUCCESS 0
#define MINFILE_ERROR -1
#define MINFILE_HEADER_SIZE 10

#include <stdint.h>

typedef struct minfile {
	uint8_t *image;
	uint32_t size;
	uint32_t entrypoint;
} minfile;

minfile *minfile_new();
int32_t minfile_load(minfile *f_st, char *path); // Error Code


#endif
