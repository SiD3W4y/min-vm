#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "core/vm.h"
#include "core/ops.h"
#include "core/syscall.h"
#include "core/minfile.h"
#include "asm/disas.h"
#include "utils/log.h"
#include "utils/memory.h"

// couple of defines usefull to make the code cleaner
#define OP_IMM_IMM 0x0
#define OP_REG_IMM 0x1
#define OP_IMM_REG 0x2
#define OP_REG_REG 0x3

int vm_load_file(vm_state *st,char *path)
{
    minfile *binary = minfile_new();
    if(minfile_load(binary,path) < 0){
        log_error("There was an error processing the file");
        return -1;
    }

    st->entrypoint = binary->entrypoint;
    st->binary_size = binary->size;
    st->total_size = binary->size; // This is just useless
    st->memory = binary->image;

    st->ip = st->entrypoint;

    if(st->debug > 0){
        log_info("Binary entry point : 0x%08x\n",st->entrypoint);
    }
    return 0;
}

int vm_load_mem(vm_state *st, unsigned char *buff, int size)
{
    st->entrypoint = buff[2];
    st->binary_size = size;
    st->total_size = size; // This is just useless
    st->memory = buff;

    st->ip = st->entrypoint;

    if(st->debug > 0){
        log_info("Binary entry point : 0x%08x\n",st->entrypoint);
    }
    return 0;
}

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
	
    st->debug = 0;
	
    st->ip = 0;
    st->regs[VM_REG_COUNT-1] = VM_MEMORY; // SP
    st->regs[VM_REG_COUNT-2] = 0; // BP
	
    st->memory = malloc(VM_MEMORY);
    memset(st->memory,0,VM_MEMORY);

    return st;
}

void vm_free(vm_state *st)
{
    free(st->memory);
    free(st);
}

char *vm_error_tostr(vm_error err)
{
    switch(err){
        case VMERR_OK:
            return "No Error";
        case VMERR_INVALID_INS:
            return "Invalid instruction";
        case VMERR_INVALID_REG:
            return "Invalid register index";
        case VMERR_INVALID_REG_INS:
            return "Invalid register/immediate combination for instruction";
        case VMERR_ARG_COMB:
            return "Impossible argument combination";
    }

    return "Why are you here ?";
}

void vm_stacktrace(vm_state *st)
{
    int i = 0;
    int cnt = st->ip;
    int off = 0;
    char asmval[50];

    printf("============ Stacktrace ============\n");

    for(i = 0;i < 15; i++) {
        if(cnt < st->binary_size){
            off += ds_disassemble((char *)&st->memory[cnt], (char *)&asmval);
            printf("0x%08x : %s\n", cnt, asmval);
            cnt += off;
        }
    }

    printf("============ Register status ============\n");
	
    for(i = 0; i < VM_REG_COUNT; i++) {
        printf("%s = 0x%08x\n", OP_REGS[i], st->regs[i]);
    }
}

int vm_syscall(vm_state *st,int syscall)
{
    switch(syscall){
        case SYS_WRITE:
            if(st->debug > 0){
                log_syscall("write(fd -> %d, addr -> 0x%08x, size -> %d)\n",st->regs[1],st->regs[2],st->regs[3]);
            }
            write(st->regs[1],&st->memory[st->regs[2]],st->regs[3]);
            break;
        case SYS_READ:
            if(st->debug > 0){
                log_syscall("read(fd -> %d, addr -> 0x%08x, size -> %d)\n",st->regs[1],st->regs[2],st->regs[3]);
            }
            read(st->regs[1],&st->memory[st->regs[2]],st->regs[3]);
            break;
        case SYS_EXIT:
            if(st->debug > 0){
                log_syscall("exit(code -> %d)\n",st->regs[1]);
            }
            exit(st->regs[2]);
    }

    return 0;
}

vm_error vm_execute(vm_state *st)
{
    uint8_t op = 0;
    uint8_t mode = 0;
    uint32_t arg0 = 0;
    uint32_t arg1 = 0;
    char asmval[50];


    while(st->ip < st->binary_size) {
    	if(st->debug == 2){
	    	ds_disassemble((char *)&st->memory[st->ip], (char *)&asmval);
	    	log_tracing("0x%08x : %s\n",st->ip,asmval);
	    }

        
        op = MEM_GET_U8(&st->memory[st->ip++]);

        if(op > OP_MAX_INDEX)
            return VMERR_INVALID_INS;

        mode = MEM_GET_U8(&st->memory[st->ip++]);

        if(mode == OP_REG_IMM) {
            // handling <op> <reg> <imm>
            arg0 = MEM_GET_U16(&st->memory[st->ip]);
            st->ip += 2;

            arg1 = MEM_GET_U32(&st->memory[st->ip]);
            st->ip += 4;

            if(arg0 >= VM_REG_COUNT)
                return VMERR_INVALID_REG;

            switch(op) {
                case OP_MOV:
                    st->regs[arg0] = arg1;
                    break;
                case OP_LDR:
                    st->regs[arg0] = MEM_GET_U32(&st->memory[arg1]);
                    break;
                case OP_LDRB:
                    st->regs[arg0] = MEM_GET_U8(&st->memory[arg1]);
                    break;
                case OP_STR:
                    MEM_SET_U32(&st->memory[arg1], st->regs[arg0]);
                    break;
                case OP_STRB:
                    MEM_SET_U8(&st->memory[arg1], st->regs[arg0]);
                    break;
                case OP_ADD:
                    st->regs[arg0] += arg1;
                    break;
                case OP_SUB:
                    st->regs[arg0] -= arg1;
                    break;
                case OP_MUL:
                    st->regs[arg0] *= arg1;
                    break;
                case OP_XOR:
                    st->regs[arg0] ^= arg1;
                    break;
                case OP_OR:
                    st->regs[arg0] |= arg1;
                    break;
                case OP_AND:
                    st->regs[arg0] &= arg1;
                    break;
                case OP_SHR:
                    st->regs[arg0] = st->regs[arg0] >> arg1;
                    break;
                case OP_SHL:
                    st->regs[arg0] = st->regs[arg0] << arg1;
                    break;
                case OP_CMP:
                    arg0 = st->regs[arg0];

                    if(arg0 == arg1) {
                        st->flags[0] = 1;
                    } else {
                        st->flags[0] = 0;
                    }

                    if(arg0 > arg1){
                        st->flags[1] = 1;
                    } else {
                        st->flags[1] = 0;
                    }

                    if(arg0 < arg1){
                        st->flags[2] = 1;
                    } else {
                        st->flags[2] = 0;
                    }
                    break;
                default:
                    return VMERR_INVALID_REG_INS;
            }

        } else if(mode == OP_REG_REG) {
            // handling <op> <reg> <reg>
            arg0 = MEM_GET_U16(&st->memory[st->ip]);
            st->ip += 2;

            arg1 = MEM_GET_U16(&st->memory[st->ip]);
            st->ip += 2;

            if(arg0 >= VM_REG_COUNT || arg1 >= VM_REG_COUNT)
                return VMERR_INVALID_REG;

            switch(op) {
                case OP_MOV:
                    st->regs[arg0] = st->regs[arg1];
                    break;
                case OP_LDR:
                    st->regs[arg0] = MEM_GET_U32(&st->memory[st->regs[arg1]]);
                    break;
                case OP_LDRB:
                    st->regs[arg0] = MEM_GET_U8(&st->memory[st->regs[arg1]]);
                    break;
                case OP_STR:
                    MEM_SET_U32(&st->memory[st->regs[arg1]], st->regs[arg0]);
                    break;
                case OP_STRB:
                    MEM_SET_U8(&st->memory[st->regs[arg1]], st->regs[arg0]);
                    break;
                case OP_ADD:
                    st->regs[arg0] += st->regs[arg1];
                    break;
                case OP_SUB:
                    st->regs[arg0] -= st->regs[arg1];
                    break;
                case OP_MUL:
                    st->regs[arg0] *= st->regs[arg1];
                    break;
                case OP_XOR:
                    st->regs[arg0] ^= st->regs[arg1];
                    break;
                case OP_OR:
                    st->regs[arg0] |= st->regs[arg1];
                    break;
                case OP_AND:
                    st->regs[arg0] &= st->regs[arg1];
                    break;
                case OP_SHR:
                    st->regs[arg0] = st->regs[arg0] >> st->regs[arg1];
                    break;
                case OP_SHL:
                    st->regs[arg0] = st->regs[arg0] << st->regs[arg1];
                    break;
                case OP_CMP:
                    arg0 = st->regs[arg0];
                    arg1 = st->regs[arg1];

                    if(arg0 == arg1) {
                        st->flags[0] = 1;
                    } else {
                        st->flags[0] = 0;
                    }

                    if(arg0 > arg1){
                        st->flags[1] = 1;
                    } else {
                        st->flags[1] = 0;
                    }

                    if(arg0 < arg1){
                        st->flags[2] = 1;
                    } else {
                        st->flags[2] = 0;
                    }
                    break;
                case OP_PUSH:
                    st->regs[REG_SP] -= 4;
                    MEM_SET_U32(&st->memory[st->regs[REG_SP]], st->regs[arg0]);
                    break;
                case OP_POP:
                    st->regs[arg0] = MEM_GET_U32(&st->memory[st->regs[REG_SP]]);
                    st->regs[REG_SP] += 4;
                    break;
                case OP_RET:
                    st->ip = MEM_GET_U32(&st->memory[st->regs[REG_SP]]);
                    st->regs[REG_SP] += 4;
                    break;
                case OP_SYS:
                    vm_syscall(st,st->regs[0]);
                    break;
                default:
                    return VMERR_INVALID_REG_INS;
            }

        } else if(mode == OP_IMM_REG) {
            // handling <op> <imm> <reg>
            arg0 = MEM_GET_U32(&st->memory[st->ip]); 
            st->ip += 4;

            arg1 = MEM_GET_U16(&st->memory[st->ip]);
            st->ip += 2;

            switch(op) {
                case OP_JMP:
                    st->ip = arg0;
                    break;
                case OP_JNE:
                    if(st->flags[0] == 0)
                        st->ip = arg0;
                    break;
                case OP_JE:
                    if(st->flags[0] == 1)
                        st->ip = arg0;
                    break;
                case OP_JLE:
                    if(st->flags[2] == 1)
                        st->ip = arg0;
                    break;
                case OP_JBE:
                    if(st->flags[1] == 1)
                        st->ip = arg0;
                    break;
                case OP_CALL:
                    st->regs[REG_SP] -= 4;
                    MEM_SET_U32(&st->memory[st->regs[REG_SP]], st->ip);
                    st->ip = arg0; 
                    break;
                default:
                    return VMERR_INVALID_REG_INS;
            }

        } else {
            // <op> <imm> <imm>
            // These are not valid argument combination
            return VMERR_ARG_COMB;
        }
    }

    return VMERR_OK;
}