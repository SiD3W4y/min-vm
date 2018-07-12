CFLAGS=-Wall -fPIC
INCLUDES=-I include

CORE_SRC=$(wildcard src/core/*.c)
ASM_SRC=$(wildcard src/asm/*.c)
UTILS_SRC=$(wildcard src/utils/*.c)

CORE_OBJ=$(patsubst src/core/%.c,src/core/%.o,$(CORE_SRC))
ASM_OBJ=$(patsubst src/asm/%.c,src/asm/%.o,$(ASM_SRC))
UTILS_OBJ=$(patsubst src/utils/%.c,src/utils/%.o,$(UTILS_SRC))

all: libminvm.so min-vm

libminvm.a: $(ASM_OBJ) $(CORE_OBJ) $(UTILS_OBJ)
	ar cr $@ $^

libminvm.so: libminvm.a
	$(CC) $< -shared -o $@

min-vm: src/min-vm.o libminvm.a
	$(CC) $^ -o $@

src/min-vm.o: src/min-vm.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/core/%.o: src/core/%.c	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/asm/%.o: src/asm/%.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


src/utils/%.o: src/utils/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(ASM_OBJ)
	rm -rf $(UTILS_OBJ)
	rm -rf $(CORE_OBJ)
	rm -rf src/*.o
	rm -rf libminvm.a
	rm -rf libminvm.so
	rm -rf min-vm
