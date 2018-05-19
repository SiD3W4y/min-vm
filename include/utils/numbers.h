#ifndef NUMBERS_H
#define NUMBERS_H

#include <stdint.h>
#include <stdlib.h>

uint32_t num_tou32(uint8_t *str);
uint32_t u32_from_stream(uint8_t *stream);
uint16_t u16_from_stream(uint8_t *stream);

#endif
