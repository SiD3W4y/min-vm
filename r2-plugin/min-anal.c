#include <r_lib.h>
#include <r_asm.h>
#include <r_anal.h>

#include "min/core/ops.h"

typedef struct minins_t {
    int opcode;
    int size;
    ut32 arg0;
    ut32 arg1;
    bool arg0_reg;
    bool arg1_reg;
} MinInstruction;

static void __parse_instruction(MinInstruction *ins, const ut8 *data)
{
    ut8 control_byte = *(data + 1);
    ut32 pptr = 0;
    
    ins->opcode = (int)(*data);
    ins->arg0_reg = (bool)(control_byte & 0x1);
    ins->arg1_reg = (bool)((control_byte >> 1) & 0x1);

    if(ins->arg0_reg) {
        pptr = 2; 
        ins->arg0 = (ut32)(*(ut16 *)(data + 2));
    } else {
        pptr = 4;
        ins->arg0 = *(ut32 *)(data + 2);
    }

    if(ins->arg1_reg) {
        ins->arg1 = (ut32)(*(ut16 *)(data + 2 + pptr));
        pptr += 2;
    } else {
        ins->arg1 = *(ut32 *)(data + 2 + pptr);
        pptr += 4;
    }

    ins->size = pptr + 2;
}

static int min_ins(RAnal *anal, RAnalOp *op, ut64 addr, const ut8 *data, int len)
{
    MinInstruction ins;
    __parse_instruction(&ins, data);

    op->addr = addr;
    op->type = R_ANAL_OP_TYPE_UNK;

    // Handling invalid opcode
    if(ins.opcode > OP_MAX_INDEX) {
        op->size = 1;
        return op->size;
    }

    op->size = ins.size;

    switch(ins.opcode){
        case OP_LDR:
        case OP_LDRB:
            op->type = R_ANAL_OP_TYPE_LOAD;
            op->ptr = ins.arg1;
            break;
        case OP_MOV:
            op->type = R_ANAL_OP_TYPE_MOV;
            break;
        case OP_STR:
        case OP_STRB:
            op->type = R_ANAL_OP_TYPE_STORE;
            break;
        case OP_SYS:
            op->type = R_ANAL_OP_TYPE_SWI;
            break;
        case OP_CALL:
            op->type = R_ANAL_OP_TYPE_CALL;
            op->jump = ins.arg0;
            break;
        case OP_JMP:
            op->type = R_ANAL_OP_TYPE_JMP;
            op->jump = ins.arg0;
            break;
        case OP_CMP:
            op->type = R_ANAL_OP_TYPE_CMP;
            break;
        case OP_JBE:
        case OP_JE:
        case OP_JLE:
        case OP_JNE:
            op->type = R_ANAL_OP_TYPE_CJMP;
            op->jump = ins.arg0;
            op->fail = addr + ins.size;
            break;
        case OP_RET:
            op->type = R_ANAL_OP_TYPE_RET;
            break;
        default:
            break;
    }

    return ins.size;
}

RAnalPlugin r_anal_plugin_min = {
	.name = "min",
	.desc = "min bytecode executable analysis plugin",
	.license = "none",
	.arch = "min",
	.bits = 32,
	.op = &min_ins,
	.esil = false,
};

#ifndef CORELIB
RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_ANAL,
	.data = &r_anal_plugin_min,
	.version = R2_VERSION
};
#endif