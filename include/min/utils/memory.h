#ifndef NUMBERS_H
#define NUMBERS_H

#include <stdint.h>
#include <stdlib.h>

#define MEM_GET_U32(ptr) *(uint32_t *)ptr
#define MEM_GET_U16(ptr) *(uint16_t *)ptr
#define MEM_GET_U8(ptr) *(uint8_t *)ptr

#define MEM_SET_U32(dest, src) *(uint32_t *)dest = (uint32_t)src
#define MEM_SET_U16(dest, src) *(uint16_t *)dest = (uint16_t)src
#define MEM_SET_U8(dest, src) *(uint8_t *)dest = (uint8_t)src

uint32_t num_tou32(char *str);

#endif
