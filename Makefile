CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Wpedantic -Werror -O3
LIBS=-lm -lSDL2 -lSDL2_ttf
INC=

CFLAGS+=$(FLAGS)
LIBS+=$(LIBRARIES)
INC+=$(INCLUDE)

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

