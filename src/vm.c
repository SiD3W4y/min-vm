#include <stdlib.h>
#include <stdio.h>

#include "vm.h"

vm_state *vm_new()
{
	int i = 0;
	vm_state *st = malloc(sizeof(vm_state));
	
	// We init the regs
	for(i = 0;i < VM_REG_COUNT;i++){
		st->regs[i] = 0;
	}

	// And the flags
	for(i = 0;i < VM_FLAG_COUNT;i++){
		st->flags[i] = 0;
	}
	
	st->ip = 0;
	st->sp = VM_MEMORY;
	st->bp = 0;

	st->memory = malloc(VM_MEMORY);

	return st;

}


