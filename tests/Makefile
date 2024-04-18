CC=clang
FLAGS := -pedantic -std=c99 -g -Wall -Wextra -fsanitize=address,leak,undefined,unreachable

all: table

table: table.c
	${CC} ${FLAGS} $^ -o $@.out

clean:
	rm -f *.out *.o
