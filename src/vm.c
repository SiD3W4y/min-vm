#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "vm.h"
#include "ops.h"
#include "disas.h"
#include "syscall.h"
#include "log.h"

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

static void vm_print_regs(vm_state *st) // Debug function ?
{
	log_info("Register state\n");
	int i;
	for(i=0;i < VM_REG_COUNT;i++){
		log_info("%s : 0x%08x\n",OP_REGS[i],st->regs[i]);
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

int vm_execute(vm_state *st)
{
	while(st->ip < st->binary_size){	
		vm_opcode op = vm_get_op(st);

		if(st->debug == 2){
			log_tracing("0x%08x : %s %d %d\n",st->ip,OP_NAMES[op.op],op.first_value,op.second_value);
		}


		switch(op.op){
			case OP_MOV:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] = st->regs[op.second_value];
					}else{
						st->regs[op.first_value] = op.second_value;
					}
				}else{
					log_error("MOV : Error wrong argument combination\n");
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
					log_error("LDR : First agument not a register\n");
					return -1;
				}
				break;
			case OP_SYS:
				vm_syscall(st,st->regs[0]);
				break;
			case OP_JMP:
				if(op.first_reg == false){
					st->ip = op.first_value;

					if(st->debug > 0){
						log_info("jmp 0x%08x\n",st->ip);
					}
				}else{
					log_error("JMP : First argument must be a value\n");
					return -1;
				}
				break;
			case OP_JNE:
				if(op.first_reg == false){
					if(st->flags[0] == 0){
						st->ip = op.first_value;
					}

					if(st->debug > 0){
						log_info("jne 0x%08x\n",st->ip);
					}

				}else{
					log_error("JNE first argument must be a value\n");
				}
				break;
			case OP_JE:
				if(op.first_reg == false){
					if(st->flags[0] == 1){
						st->ip = op.first_value;
					}

					if(st->debug > 0){
						log_info("je 0x%08x\n",st->ip);
					}

				}else{
					log_error("JE first argument must be a value\n");
				}
				break;
			case OP_JLE:
				if(op.first_reg == false){
					if(st->flags[2] == 1){
						st->ip = op.first_value;
					}

					if(st->debug > 0){
						log_info("jle 0x%08x\n",st->ip);
					}

				}else{
					log_error("JLE first argument must be a value\n");
				}
				break;
			case OP_JBE:
				if(op.first_reg == false){
					if(st->flags[1] == 1){
						st->ip = op.first_value;
					}

					if(st->debug > 0){
						log_info("jbe 0x%08x\n",st->ip);
					}

				}else{
					log_error("JBE first argument must be a value\n");
				}
				break;





			case OP_ADD:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] += st->regs[op.second_value];
					}else{
						st->regs[op.first_value] += op.second_value;
					}
				}else{
					log_error("ADD first argument must be a register !\n");
					return -1;
				}
				break;

			case OP_SUB:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] -= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] -= op.second_value;
					}
				}else{
					log_error("SUB first argument must be a register !\n");
					return -1;
				}
				break;

			case OP_MUL:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] *= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] *= op.second_value;
					}
				}else{
					log_error("MUL first argument must be a register !\n");
					return -1;
				}
				break;

			case OP_XOR:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] ^= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] ^= op.second_value;
					}

				}else{
					log_error("XOR first argument must be a register !\n");
					return -1;
				}
				break;
			case OP_OR:
			if(op.first_reg == true){
				if(op.second_reg == true){
					st->regs[op.first_value] |= st->regs[op.second_value];
				}else{
					st->regs[op.first_value] |= op.second_value;
				}

			}else{
				log_error("OR first argument must be a register !\n");
				return -1;
			}
			break;
			case OP_AND:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] &= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] &= op.second_value;
					}

				}else{
					log_error("AND first argument must be a register !\n");
					return -1;
				}
				break;
			case OP_SHR:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] >>= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] >>= op.second_value;
					}

				}else{
					log_error("SHR first argument must be a register !\n");
					return -1;
				}
				break;
			case OP_SHL:
				if(op.first_reg == true){
					if(op.second_reg == true){
						st->regs[op.first_value] <<= st->regs[op.second_value];
					}else{
						st->regs[op.first_value] <<= op.second_value;
					}

				}else{
					log_error("SHL first argument must be a register !\n");
					return -1;
				}
				break;
			case OP_CMP:
				if(op.first_reg == true){
					// st->flags[0] is Z flag : ==
					// st->flags[1] is H flag : >
					// st->flags[2] is L flag : <
					int lhs = st->regs[op.first_value];
					int rhs = op.second_value;

					if(op.second_reg == true){
						rhs = st->regs[rhs];
					}	
					
					

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

					if(st->debug > 0){
						log_info("0x%08x : CMP %d %d [Z = %d,H = %d,L = %d]\n",st->ip,lhs,rhs,st->flags[0],st->flags[1],st->flags[2]);
					}
				}else{
					log_error("CMP first argument must be a register !\n");
				}
				break;
		}
	}

	return 0;
}


