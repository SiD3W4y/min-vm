NAME=min
R2_PLUGIN_PATH=$(shell r2 -hh|grep LIBR_PLUGINS|awk '{print $$2}')
CFLAGS=-g -fPIC $(shell pkg-config --cflags r_asm) -I ../include
LDFLAGS=-shared $(shell pkg-config --libs r_asm)
OBJS=min-ds.o
SO_EXT=$(shell uname|grep -q Darwin && echo dylib || echo so)

ASM_SRC=$(wildcard ../src/asm/*.c)
ASM_OBJ=$(patsubst ../src/asm/%.c,../build/asm/%.o,$(ASM_SRC))

LIBS=min-disas.$(SO_EXT) min-anal.$(SO_EXT)

all: $(LIBS)

min-disas.$(SO_EXT): min-disas.o
	$(CC) $(LDFLAGS) $^ ../libminvm.a -shared -o $@

min-anal.$(SO_EXT): min-anal.o
	$(CC) $(LDFLAGS) $^ ../libminvm.a -shared -o $@

clean:
	rm -f *.o
	rm -f *.so

install:
	cp -f *.$(SO_EXT) $(R2_PLUGIN_PATH)/

uninstall:
	rm -f $(R2_PLUGIN_PATH)/$(NAME).$(SO_EXT)
