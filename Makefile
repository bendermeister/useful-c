FLAGS := -std=c99 -Wall -Wextra -pedantic -I src
DEBUG_FLAGS := ${FLAGS} -g -fsanitize=address,leak,undefined,unreachable -DDEBUG

all: test example
	echo "Useful C"

TEST := test/vec.out test/table.out test/arena.out
EXAMPLE := example/error/error.out example/ucx/ucx.out

test: ${TEST}

example: ${EXAMPLE}

%.out: %.c
	${CC} ${DEBUG_FLAGS} $< -o $@

example/ucx/ucx.out: example/ucx/*
	${CC} ${DEBUG_FLAGS} -c example/ucx/ucx.impl.c -o example/ucx/ucx.impl.o
	${CC} ${DEBUG_FLAGS} -c example/ucx/main.c -o example/ucx/main.o
	${CC} ${DEBUG_FLAGS} example/ucx/main.o example/ucx/ucx.impl.o -o example/ucx/ucx.out

INSTALL_DIR := /usr/local/include

install:
	cp -r src/uc ${INSTALL_DIR}

uninstall:
	rm -rf ${INSTALL_DIR}/uc

tidy:
	clang-tidy --checks=cert-* src/uc/*.h src/uc/*.c test/*.c test/*.h example/*/*.c

clean:
	rm -f ./*/*.out ./example/*/*.out ./example/*/*.o
