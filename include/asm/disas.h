#ifndef DISAS_H
#define DISAS_H

#include "core/ops.h"

extern const char *OP_NAMES[];
extern const char *OP_REGS[];

void ds_print_op(vm_opcode op);

#endif
