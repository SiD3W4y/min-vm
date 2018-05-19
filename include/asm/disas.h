#ifndef DISAS_H
#define DISAS_H

#include <stdint.h>

#include "core/ops.h"

extern const char *OP_NAMES[];
extern const char *OP_REGS[];

void ds_print_op(vm_opcode op);
uint32_t ds_disassemble(char *input_bytes, char *output);

#endif
