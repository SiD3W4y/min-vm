#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "vm.h"
#include "ops.h"
#include "disas.h"
#include "syscall.h"


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

static void vm_print_regs(vm_state *st)
{
	int i;
	for(i=0;i < VM_REG_COUNT;i++){
		printf("%d : 0x%08x\n",i,st->regs[i]);
	}
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
	st->binary_size = bytes_to_int(&st->memory[6]);

	//printf("[+] Data size : 0x%x Entrypoint : 0x%x\n",st->binary_size,st->entrypoint);
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
	
	st->debug = 0;
	
	st->ip = 0;
	st->regs[VM_REG_COUNT-1] = VM_MEMORY; // SP
	st->regs[VM_REG_COUNT-2] = 0; // BP
	
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

int vm_syscall(vm_state *st,int syscall)
{
	switch(syscall){
		case SYS_WRITE:
//			printf("write(fd -> %d, addr -> 0x%08x, size -> %d)\n",st->regs[1],st->regs[2],st->regs[3]);
			write(st->regs[1],&st->memory[st->regs[2]],st->regs[3]);
			break;
		case SYS_EXIT:
//			printf("exit(code -> %d)\n",st->regs[2]);
			exit(st->regs[2]);
	}

	return 0;
}

int vm_execute(vm_state *st)
{
	while(st->ip < st->binary_size){	
		vm_opcode op = vm_get_op(st);


		switch(op.op){
			case OP_MOV:
				if(op.first_reg == true && op.second_reg == false){
					st->regs[op.first_value] = op.second_value;
				}else{
					printf("MOV : Error wrong argument combination\n");
					return -1;
				}
				break;
			case OP_LDR:
				if(op.first_reg == true){
					if(op.second_reg == true){
						memcpy(&st->regs[op.first_value],&st->memory[st->regs[op.second_value]],4);
					}else{
						memcpy(&st->regs[op.first_value],&st->memory[op.second_value],4);
					}
				}else{
					printf("LDR : First agument not a register\n");
					return -1;
				}
				break;
			case OP_SYS:
				vm_syscall(st,st->regs[0]);
				break;
			case OP_JMP:
				if(op.first_reg == false){
					//printf("Jump to 0x%08x\n",st->ip);
					st->ip = op.first_value;
				}else{
					printf("JMP : First argument must be a value\n");
					return -1;
				}
				break;
		}
	}

	return 0;
}


