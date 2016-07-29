CC?=gcc
cflags=-I./src $(shell pkg-config libuv --cflags --libs) -lrt -ldl -std=gnu99 -Wall -pedantic -Werror
sources=$(wildcard src/*.c)
objects := $(patsubst src/%.c,build/%.o,$(sources))

all: bin/client

re: clean all

bin/client: ${objects} build/client-cmd.o
	@mkdir -p bin
	@${CC} ${objects} build/client-cmd.o ${cflags} -o $@

clean:
	@rm -rf build/*.o

build/%.o: src/%.c
	@mkdir -p build
	@${CC} ${cflags} -c $< -O3  -o $@

build/%-cmd.o: src/cmd/%.c
	@mkdir -p build
	@${CC} ${cflags} -c $< -O3  -o $@
