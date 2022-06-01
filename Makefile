CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Wpedantic -Werror
LIBS=-lm -lSDL2

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

all:
	mkdir -p obj/src
	$(MAKE) spherefield

spherefield: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm -rf obj a.out
