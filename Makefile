CFLAGS=-Wall
INCLUDES= -I include

SOURCES=$(wildcard src/*.c)
OBJS=$(patsubst src/%.c,build/%.o,$(SOURCES))

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o min-vm

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm build/*
	rm min-vm
