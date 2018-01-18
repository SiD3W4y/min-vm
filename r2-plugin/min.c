#include <r_asm.h>
#include <r_lib.h>

#include "asm/disas.h"

static int disassemble(RAsm *a,RAsmOp *op,const ut8 *b,int l)
{
	op->size = ds_disassemble(b,op->buf_asm);
	return op->size;
}

RAsmPlugin r_asm_plugin_zpu = {
	.name = "min",
	.arch = "min",
	.license = "none",
	.bits = 32,
	.desc = "min bytecode disassembler",
	.disassemble = &disassemble,
};

#ifndef CORELIB
RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_ASM,
	.data = &r_asm_plugin_zpu
};
#endif
