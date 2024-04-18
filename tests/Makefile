CC=clang
FLAGS := -std=c99 -g -Wall -Wextra -Werror -fsanitize=address,leak,undefined,unreachable

all: table

table: table.c allocator.o
	${CC} ${FLAGS} $^ -o $@.out

allocator.o: ../src/allocator/src/*
	${CC} ${FLAGS} -c $^ -o $@

clean:
	rm -f *.out *.o
