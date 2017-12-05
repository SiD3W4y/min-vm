#include <stdint.h>

#include "asm/disas.h"
#include "core/minfile.h"

int main()
{
	uint8_t ins[64];
	uint32_t bytes_read = 0;
	uint32_t offset = 10;

	minfile *f = minfile_new();
	minfile_load(f,"samples/arith.mx");

	printf("File size : %d\n",f->size);

	while(offset < f->size){
		bytes_read = ds_disassemble(&f->image[offset],&ins);
		printf("0x%08x (%d): %s\n",offset,bytes_read,ins);
		offset += bytes_read;
	}

	return 0;
}
