#ifndef VM_H
#define VM_H

#define VM_MEMORY 1024*42 // 42Ko
#define VM_REG_COUNT 6
#define VM_FLAG_COUNT 3

typedef struct st_vm_state{
	int regs[VM_REG_COUNT]; // A B C D E F
	int ip;
	int sp;
	int bp;
	unsigned char flags[VM_FLAG_COUNT]; // Z H L
	unsigned char *memory;

	int data_size;
	int entrypoint;
	int total_size;
}vm_state;

vm_state *vm_new();
int vm_load_file(vm_state *st,char *path);
int vm_execute();


#endif
