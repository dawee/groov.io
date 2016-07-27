CC?=gcc
CFLAGS?=-luv -lrt -ldl -std=gnu99 -Wall -pedantic -Werror

all: main.c
	${CC} main.c ${CFLAGS} -O3 -o main
