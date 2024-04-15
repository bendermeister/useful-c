FLAGS := -g -std=c2x -Wall -Werror -Wextra -fsanitize=address,leak,undefined,unreachable

all: main vec allocator
	${CC} ${FLAGS} main.o vec.o allocator.o

main: main.c
	${CC} ${FLAGS} -c main.c -o main.o

allocator: ../../allocator/src/*
	${CC} ${FLAGS} -c ../../allocator/src/* -o allocator.o

vec: ../../vec/src/*
	${CC} ${FLAGS} -c ../../vec/src/* -o vec.o

clean:
	rm -f *.out *.o
