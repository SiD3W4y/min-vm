CFLAGS=-Wall --std=c99
INCLUDES= -I include

CORE_SRC=$(wildcard src/core/*.c)
ASM_SRC=$(wildcard src/asm/*.c)
UTILS_SRC=$(wildcard src/utils/*.c)

CORE_OBJ=$(patsubst src/core/%.c,build/core/%.o,$(CORE_SRC))
ASM_OBJ=$(patsubst src/asm/%.c,build/asm/%.o,$(ASM_SRC))
UTILS_OBJ=$(patsubst src/utils/%.c,build/utils/%.o,$(UTILS_SRC))

min-vm: $(ASM_OBJ) $(CORE_OBJ) $(UTILS_OBJ) build/min-vm.o
	$(CC) $(ASM_OBJ) $(CORE_OBJ) $(UTILS_OBJ) build/min-vm.o -o min-vm

build/min-vm.o: src/min-vm.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/core/%.o: src/core/%.c build/core
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/asm/%.o: src/asm/%.c build/asm
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


build/utils/%.o: src/utils/%.c build/utils
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/core:
	@mkdir -p build/core

build/asm:
	@mkdir -p build/asm

build/utils:
	@mkdir -p build/utils

clean:
	rm -rf build
