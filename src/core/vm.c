#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "core/vm.h"
#include "core/ops.h"
#include "core/syscall.h"
#include "core/minfile.h"
#include "asm/disas.h"
#include "utils/log.h"

#define VM_GET_BYTE(vm) vm->memory[vm->ip++]

// First argument is always a register or value
#define VM_ARG_REG(op) if(!op.first_reg){\
	log_error("Wrong argument on op 0x%02x (REG expected)\n",op.op);\
	return -1;}

#define VM_ARG_VAL(op) if(op.first_reg){\
	log_error("Wrong argument on op 0x%02x (VAL expected)\n",op.op);\
	return -1;}

int vm_load_file(vm_state *st,char *path)
{
	minfile *binary = minfile_new();
	if(minfile_load(binary,path) < 0){
		log_error("There was an error processing the file");
		return -1;
	}

	st->entrypoint = binary->entrypoint;
	st->binary_size = binary->size;
	st->total_size = binary->size; // This is just useless
	st->memory = binary->image;

	st->ip = st->entrypoint;

	if(st->debug > 0){
		log_info("Binary entry point : 0x%08x\n",st->entrypoint);
	}
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

void vm_free(vm_state *st)
{
	free(st->memory);
	free(st);
}

static void vm_get_op(vm_state *st,vm_opcode *opc)
{
	uint8_t control_op;
	opc->op = VM_GET_BYTE(st); // Get opcode
	control_op = VM_GET_BYTE(st);

	opc->first_reg = control_op & 0x01;
	opc->second_reg = (control_op >> 1) & 0x01;

	if(opc->first_reg){
		opc->first_value = (uint16_t)(*&st->memory[st->ip]);
		st->ip += 2;
	}else{
		opc->first_value = (uint32_t)(*&st->memory[st->ip]);
		st->ip += 4;
	}

	if(opc->second_reg){
		opc->second_value = (uint16_t)(*&st->memory[st->ip]);
		st->ip += 2;
	}else{
		opc->second_value = (uint32_t)(*&st->memory[st->ip]);
		st->ip += 4;
	}
}

int vm_syscall(vm_state *st,int syscall)
{
	switch(syscall){
		case SYS_WRITE:
			if(st->debug > 0){
				log_syscall("write(fd -> %d, addr -> 0x%08x, size -> %d)\n",st->regs[1],st->regs[2],st->regs[3]);
			}
			write(st->regs[1],&st->memory[st->regs[2]],st->regs[3]);
			break;
		case SYS_EXIT:
			if(st->debug > 0){
				log_syscall("exit(code -> %d)\n",st->regs[1]);
			}
			exit(st->regs[2]);
	}

	return 0;
}

// Fully runs a program
int vm_execute(vm_state *st)
{
	int status = 0;
	while(st->ip < st->binary_size && status == 0){
		status = vm_step(st);
	}

	if(status != 0)
		log_error("Execution aborted\n");
	
	return status;
}

// Executes only one instruction
int vm_step(vm_state *st)
{
	vm_opcode op;
	vm_get_op(st,&op);

	if(st->debug == 2){
		log_tracing("0x%08x : %s %d %d\n",st->ip,OP_NAMES[op.op],op.first_value,op.second_value);
	}


	switch(op.op){
		case OP_MOV:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				st->regs[op.first_value] = st->regs[op.second_value];
			else
				st->regs[op.first_value] = op.second_value;

			break;
		case OP_LDR:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				memcpy(&st->regs[op.first_value],&st->memory[st->regs[op.second_value]],4);
			else
				memcpy(&st->regs[op.first_value],&st->memory[op.second_value],4);
			
			break;
		case OP_LDRB:
			VM_ARG_REG(op);
			st->regs[op.first_value] ^= st->regs[op.first_value];
			
			if(op.second_reg)
				memcpy(&st->regs[op.first_value],&st->memory[st->regs[op.second_value]],1);
			else
				memcpy(&st->regs[op.first_value],&st->memory[op.second_value],1);
			
			break;		
		case OP_STR:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				memcpy(&st->memory[st->regs[op.second_value]],&st->regs[op.first_value],4);
			else
				memcpy(&st->memory[op.second_value],&st->regs[op.first_value],4);
			
			break;
		case OP_STRB:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				memcpy(&st->memory[st->regs[op.second_value]],&st->regs[op.first_value],1);
			else
				memcpy(&st->memory[op.second_value],&st->regs[op.first_value],1);
				
			break;
		case OP_SYS:
			vm_syscall(st,st->regs[0]);
			break;
		case OP_JMP:
			VM_ARG_VAL(op);
			st->ip = op.first_value;

			if(st->debug > 0)
				log_info("jmp 0x%08x\n",st->ip);

			break;
		case OP_JNE:
			VM_ARG_VAL(op);
			
			if(st->flags[0] == 0)
				st->ip = op.first_value;

			if(st->debug > 0)
				log_info("jne 0x%08x\n",st->ip);
				
			break;
		case OP_JE:
			VM_ARG_VAL(op);
			
			if(st->flags[0] == 1)
				st->ip = op.first_value;
			
			if(st->debug > 0)
				log_info("je 0x%08x\n",st->ip);
			
			break;
		case OP_JLE:
			VM_ARG_VAL(op);

			if(st->flags[2] == 1)
				st->ip = op.first_value;

			if(st->debug > 0)
				log_info("jle 0x%08x\n",st->ip);
			
			break;
		case OP_JBE:
			VM_ARG_VAL(op);

			if(st->flags[1] == 1)
				st->ip = op.first_value;

			if(st->debug > 0)
				log_info("jbe 0x%08x\n",st->ip);

			break;

		case OP_ADD:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				st->regs[op.first_value] += st->regs[op.second_value];
			else
				st->regs[op.first_value] += op.second_value;
			
			break;

		case OP_SUB:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] -= st->regs[op.second_value];
			else
				st->regs[op.first_value] -= op.second_value;
		
			break;

		case OP_MUL:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] *= st->regs[op.second_value];
			else
				st->regs[op.first_value] *= op.second_value;

			break;

		case OP_XOR:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] ^= st->regs[op.second_value];
			else
				st->regs[op.first_value] ^= op.second_value;

			break;
		case OP_OR:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] |= st->regs[op.second_value];
			else
				st->regs[op.first_value] |= op.second_value;

			break;
		case OP_AND:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] &= st->regs[op.second_value];
			else
				st->regs[op.first_value] &= op.second_value;

			break;
		case OP_SHR:
			VM_ARG_REG(op);
			
			if(op.second_reg)
				st->regs[op.first_value] >>= st->regs[op.second_value];
			else
				st->regs[op.first_value] >>= op.second_value;

			break;
		case OP_SHL:
			VM_ARG_REG(op);

			if(op.second_reg)
				st->regs[op.first_value] <<= st->regs[op.second_value];
			else
				st->regs[op.first_value] <<= op.second_value;

			break;
		case OP_CMP:
			VM_ARG_REG(op);
			// st->flags[0] is Z flag : ==
			// st->flags[1] is H flag : >
			// st->flags[2] is L flag : <
			int lhs = st->regs[op.first_value];
			int rhs = op.second_value;

			if(op.second_reg)
				rhs = st->regs[rhs];
			
			if(lhs == rhs){
				st->flags[0] = 1;
			}else{
				st->flags[0] = 0;
			}

			if(lhs > rhs){
				st->flags[1] = 1;
			}else{
				st->flags[1] = 0;
			}

			if(lhs < rhs){
				st->flags[2] = 1;
			}else{
				st->flags[2] = 0;
			}

			if(st->debug > 0)
				log_info("0x%08x : CMP %d %d [Z = %d,H = %d,L = %d]\n",st->ip,lhs,rhs,st->flags[0],st->flags[1],st->flags[2]);
			break;

		case OP_PUSH:
			VM_ARG_REG(op);
			
			st->regs[REG_SP] -= sizeof(uint32_t);
			memcpy(&st->memory[st->regs[REG_SP]],&st->regs[op.first_value],sizeof(uint32_t));
			
			break;
		case OP_POP:
			VM_ARG_REG(op);
			
			memcpy(&st->regs[op.first_value],&st->memory[st->regs[REG_SP]],sizeof(uint32_t));
			st->regs[REG_SP] += sizeof(uint32_t);

			break;
	}

	return 0;
}


