#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "asm/disas.h"
#include "core/ops.h"

const char* OP_NAMES[] = {"ADD","SUB","MUL","MOV","LDR","LDRB","STR","STRB","PUSH","POP","CMP","JMP","JNE","JE","JLE","JBE","SYS","XOR","AND","OR","SHR","SHL"};
const char *OP_REGS[] = {"A","B","C","D","E","F","BP","SP"};

static void parse_cond(vm_opcode *op,unsigned char data)
{
	op->first_reg = data & 0x01;
	op->second_reg = (data >> 1) & 0x01;
}


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



// returns the number of bytes read (processed)
// TODO : Clean the return value thingy
uint32_t ds_disassemble(uint8_t *input_bytes,uint8_t *output)
{
	vm_opcode op;
	
	op.op = input_bytes[0];

	if(op.op > OP_MAX_INDEX){
		sprintf(output,"invalid");
		return 1;
	}
	
	parse_cond(&op,input_bytes[1]);
	
	if(op.first_reg == true && op.second_reg == true){
		op.first_value = (uint16_t)(*&input_bytes[2]);
		op.second_value = (uint16_t)(*&input_bytes[4]);

		sprintf(output,"%s $%s $%s",OP_NAMES[op.op],OP_REGS[op.first_value],OP_REGS[op.second_value]);

		return 6;
	}

	if(op.first_reg == false && op.second_reg == true){
		op.first_value = (uint32_t)(*&input_bytes[2]);
		op.second_value = (uint16_t)(*&input_bytes[6]);

		sprintf(output,"%s 0x%08x $%s",OP_NAMES[op.op],op.first_value,OP_REGS[op.second_value]);

		return 8;
	}

	if(op.first_reg == true && op.second_reg == false){
		op.first_value = (uint16_t)(*&input_bytes[2]);
		op.second_value = (uint32_t)(*&input_bytes[4]);

		sprintf(output,"%s $%s 0x%08x",OP_NAMES[op.op],OP_REGS[op.first_value],op.second_value);

		return 8;
	}

	if(op.first_reg == true && op.second_reg == true){
		op.first_value = (uint32_t)(*&input_bytes[2]);
		op.second_value = (uint32_t)(*&input_bytes[6]);

		sprintf(output,"%s 0x%08x 0x%08x",OP_NAMES[op.op],op.first_value,op.second_value);

		return 10;
	}

	return 1;
}


