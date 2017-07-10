#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "vm.h"
#include "ops.h"


#define VM_GET_BYTE(vm) vm->memory[vm->ip++]

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

static short bytes_to_short(unsigned char *buff)
{
	short result = 0;

	result |= buff[1];
	result = result << 8;
	result |= buff[0];

	return result;
}

static void parse_cond(vm_opcode *op,unsigned char data)
{
	op->first_reg = data & 0x01;
	op->second_reg = (data >> 1) & 0x01;
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
	st->ip = st->entrypoint;

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

vm_opcode vm_get_op(vm_state *st)
{
	vm_opcode opc;

	opc.op = VM_GET_BYTE(st); // Get opcode
	parse_cond(&opc,VM_GET_BYTE(st)); // Parse them and put the result in opcode struct

	if(opc.first_reg == true){
		opc.first_value = bytes_to_short(&st->memory[st->ip]);
		st->ip += 2;
	}else{
		opc.first_value = bytes_to_int(&st->memory[st->ip]);
		st->ip += 4;
	}

	if(opc.second_reg == true){
		opc.second_value = bytes_to_short(&st->memory[st->ip]);
		st->ip += 2;
	}else{
		opc.second_value = bytes_to_int(&st->memory[st->ip]);
		st->ip += 4;
	}

	return opc;
}

int vm_execute(vm_state *st)
{
	vm_opcode op = vm_get_op(st);
	return 0;
}


