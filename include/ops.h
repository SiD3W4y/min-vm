#ifndef OPS_H
#define OPS_H

#define OP_ADD 0x0                              
#define OP_SUB 0x1                              
#define OP_MUL 0x2                              
#define OP_MOV 0x3                              
#define OP_LDR 0x4                              
#define OP_LDRB 0x5                             
#define OP_STR 0x6                              
#define OP_STRB 0x7                             
#define OP_PUSH 0x8                             
#define OP_POP 0x9                              
#define OP_CMP 0xa                              
#define OP_JMP 0xb                              
#define OP_JNE 0xc                              
#define OP_JLE 0xd                              
#define OP_JBE 0xe                              
#define OP_SYS 0xf                              
#define OP_XOR 0x10                             
#define OP_AND 0x11                             
#define OP_SHR 0x12                             
#define OP_SHL 0x13

#include <stdbool.h>

typedef struct vm_opcode{
	int op;
	bool first_reg;
	bool second_reg;
	int first_value;
	int second_value;
}vm_opcode;


#endif
