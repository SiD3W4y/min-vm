NAME=min
R2_PLUGIN_PATH=$(shell r2 -hh|grep LIBR_PLUGINS|awk '{print $$2}')
CFLAGS=-g -fPIC $(shell pkg-config --cflags r_asm) -I ../include
LDFLAGS=-shared $(shell pkg-config --libs r_asm)
OBJS=$(NAME).o ../src/asm/disas.o ../src/utils/numbers.o
SO_EXT=$(shell uname|grep -q Darwin && echo dylib || echo so)
LIB=$(NAME).$(SO_EXT)

ASM_SRC=$(wildcard ../src/asm/*.c)
ASM_OBJ=$(patsubst ../src/asm/%.c,../build/asm/%.o,$(ASM_SRC))

all: $(LIB)

clean:
	rm -f $(LIB) $(OBJS)

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(LIB)

../build/asm/%.o: ../src/asm/%.c ../build/asm
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

../build/asm:
	mkdir -p ../build/asm

install:
	cp -f $(NAME).$(SO_EXT) $(R2_PLUGIN_PATH)

uninstall:
	rm -f $(R2_PLUGIN_PATH)/$(NAME).$(SO_EXT)
