CC?=gcc
CFLAGS?=$(shell pkg-config libuv --cflags --libs) -lrt -ldl -std=gnu99 -Wall -pedantic -Werror

all:
	@mkdir -p build
	@${CC} client.c ${CFLAGS} -O3 -o build/client
	@${CC} server.c ${CFLAGS} -O3 -o build/server

