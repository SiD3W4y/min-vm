#include <stdio.h>
#include <stdbool.h>

#include "asm/disas.h"
#include "core/ops.h"

const char* OP_NAMES[] = {"ADD","SUB","MUL","MOV","LDR","LDRB","STR","STRB","PUSH","POP","CMP","JMP","JNE","JE","JLE","JBE","SYS","XOR","AND","OR","SHR","SHL"};
const char *OP_REGS[] = {"A","B","C","D","E","F","BP","SP"};

void ds_print_op(vm_opcode op)
{
	char *name = OP_NAMES[op.op];
	if(op.first_reg == true && op.second_reg == true){
		printf("%s $%s $%s {r,r}\n",name,OP_REGS[op.first_value],OP_REGS[op.second_value]);
	}

	if(op.first_reg == true && op.second_reg == false){
		printf("%s $%s 0x%08x {r,v}\n",name,OP_REGS[op.first_value],op.second_value);
	}

	if(op.first_reg == false && op.second_reg == true){
		printf("%s 0x%08x %s\n {v,r}",name,op.first_value,OP_REGS[op.second_value]);
	}

	if(op.first_reg == false && op.second_reg == false){
		printf("%s 0x%08x 0x%08x {r,r}\n",name,op.first_value,op.second_value);
	}
}
