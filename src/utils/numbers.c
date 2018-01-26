#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils/numbers.h"

uint32_t num_tou32(uint8_t *str)
{	
	if(strlen(str) > 2){
		if(str[0] == '0' && str[1] == 'x')
			return strtoul(str,NULL,16);
	}

	return strtoul(str,NULL,10);
}

uint32_t u32_from_stream(uint8_t *stream)
{
	uint32_t *nbr_ptr = stream;
	return *nbr_ptr;
}

uint16_t u16_from_stream(uint8_t *stream)
{
	uint16_t *nbr_ptr = stream;
	return *nbr_ptr;
}
