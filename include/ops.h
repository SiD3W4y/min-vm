#ifndef OPS_H
#define OPS_H

#define OP_SYS 0x01
#define OP_ADD 0x02
#define OP_SUB 0x03
#define OP_MUL 0x04
#define OP_MOV 0x05
#define OP_LDR 0x06
#define OP_PUSH 0x07
#define OP_POP 0x08
#define OP_CMP 0x09
#define OP_JMP 0x0a

#include <stdbool.h>

typedef struct vm_opcode{
	int op;
	bool first_reg;
	bool second_reg;
	int first_value;
	int second_value;
}vm_opcode;


#endif
