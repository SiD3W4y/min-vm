#ifndef VM_H
#define VM_H

#include "core/ops.h"

#define VM_MEMORY 1024*42 // 42Ko
#define VM_REG_COUNT 8
#define VM_FLAG_COUNT 3

#define REG_BP 0x6
#define REG_SP 0x7

typedef enum vm_error_t {
    VMERR_OK,
    VMERR_ARG_COMB,
    VMERR_INVALID_INS, // invalid opcode
    VMERR_INVALID_REG, // invalid register value
    VMERR_INVALID_REG_INS // invalid argument combination for instruction
} vm_error;

typedef struct st_vm_state{
	int regs[VM_REG_COUNT]; // A B C D E F BP SP
	int ip;
	unsigned char flags[VM_FLAG_COUNT]; // Z H L
	unsigned char *memory;

	int binary_size;
	int entrypoint;
	int total_size;
	int debug; // Flag indicating if we are running in tracing (2), simple debug (1) or nothing (0) mode
}vm_state;

vm_state *vm_new();
void vm_free(vm_state *st);
int vm_load_file(vm_state *st,char *path);
vm_error vm_execute(vm_state *st);
int vm_syscall(vm_state *st,int syscall);
char *vm_error_tostr(vm_error err);

#endif