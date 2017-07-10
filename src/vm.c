#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vm.h"

static int bytes_to_int(unsigned char *buff)
{
	int result = 0;

	result |= buff[3];
	result = result << 8;
	result |= buff[2];
	result = result << 8;
	result |= buff[1];
	result = result << 8;
	result |= buff[0];

	return result;
}

int vm_load_file(vm_state *st,char *path)
{
	FILE *fp;

	fp = fopen(path,"rb");
	
	if(fp == NULL){
		perror("fopen");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	st->total_size = ftell(fp);

	rewind(fp);
	fread(st->memory,st->total_size,1,fp);
	fclose(fp);

	if(st->memory[0] != 'M' || st->memory[1] != 'X'){
		printf("This file is not a valid min bytecode executable\n");
		return -1;
	}

	st->entrypoint = bytes_to_int(&st->memory[2]);
	st->data_size = bytes_to_int(&st->memory[6]);

	printf("[+] Data size : 0x%x Entrypoint : 0x%x\n",st->data_size,st->entrypoint);

	return 0;
}

vm_state *vm_new()
{
	int i = 0;
	vm_state *st = malloc(sizeof(vm_state));
	
	// We init the regs
	for(i = 0;i < VM_REG_COUNT;i++){
		st->regs[i] = 0;
	}

	// And the flags
	for(i = 0;i < VM_FLAG_COUNT;i++){
		st->flags[i] = 0;
	}
	
	st->ip = 0;
	st->sp = VM_MEMORY;
	st->bp = 0;

	st->memory = malloc(VM_MEMORY);
	memset(st->memory,0,VM_MEMORY);

	return st;
}

int vm_execute()
{

}


