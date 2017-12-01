CFLAGS=-Wall --std=c99
INCLUDES= -I include

CORE_SRC=$(wildcard src/core/*.c)
ASM_SRC=$(wildcard src/asm/*.c)
UTILS_SRC=$(wildcard src/utils/*.c)

VM_OBJS=build/utils.o build/core.o build/asm.o build/main.o

SOURCES=$(wildcard src/*.c)
OBJS=$(patsubst src/%.c,build/%.o,$(SOURCES))

build/main.o: src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

build/core.o: $(CORE_SRC)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

build/asm.o: $(ASM_SRC)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

build/utils.o: $(UTILS_SRC)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

min-vm: $(VM_OBJS)
	$(CC) $(INCLUDES) $(CFLAGS) $(VM_OBJS) -o min-vm

clean:
	rm build/*
	rm min-vm
