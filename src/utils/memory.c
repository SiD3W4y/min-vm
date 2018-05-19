#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils/memory.h"

uint32_t num_tou32(char *str)
{	
	if(strlen(str) > 2){
		if(str[0] == '0' && str[1] == 'x')
			return strtoul(str,NULL,16);
	}
	return strtoul(str,NULL,10);
}