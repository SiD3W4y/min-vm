#include <r_asm.h>
#include <r_lib.h>

#include "asm/disas.h"

static int disassemble(RAsm *a,RAsmOp *op,const ut8 *b,int l)
{
	op->size = ds_disassemble(b,op->buf_asm);
	
	return op->size;
}

RAsmPlugin r_asm_plugin_min = {
	.name = "min",
	.arch = "min",
	.license = "none",
	.bits = 32,
	.desc = "min bytecode disassembler",
	.disassemble = &disassemble
};

#ifndef CORELIB
struct r_lib_struct_t radare_plugin = {
	.type = R_LIB_TYPE_ASM,
	.data = &r_asm_plugin_min,
	.version = R2_VERSION
};
#endif
