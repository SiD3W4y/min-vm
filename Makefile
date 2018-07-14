CFLAGS=-Wall -fPIC
INCLUDES=-I include

CORE_SRC=$(wildcard src/min/core/*.c)
ASM_SRC=$(wildcard src/min/asm/*.c)
UTILS_SRC=$(wildcard src/min/utils/*.c)

CORE_OBJ=$(patsubst src/min/core/%.c,src/min/core/%.o,$(CORE_SRC))
ASM_OBJ=$(patsubst src/min/asm/%.c,src/min/asm/%.o,$(ASM_SRC))
UTILS_OBJ=$(patsubst src/min/utils/%.c,src/min/utils/%.o,$(UTILS_SRC))

all: libminvm.so min-vm

libminvm.a: $(ASM_OBJ) $(CORE_OBJ) $(UTILS_OBJ)
	ar cr $@ $^

libminvm.so: $(ASM_OBJ) $(CORE_OBJ) $(UTILS_OBJ)
	$(CC) $^ -shared -o $@

min-vm: src/min/min-vm.o libminvm.a
	$(CC) $^ -o $@

src/min/min-vm.o: src/min/min-vm.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/min/core/%.o: src/min/core/%.c	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/min/asm/%.o: src/min/asm/%.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


src/min/utils/%.o: src/min/utils/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(ASM_OBJ)
	rm -rf $(UTILS_OBJ)
	rm -rf $(CORE_OBJ)
	rm -rf src/min/*.o
	rm -rf libminvm.a
	rm -rf libminvm.so
	rm -rf min-vm
