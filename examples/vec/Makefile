FLAGS := -g -std=c2x -Wall -Werror -Wextra -fsanitize=address,leak,undefined,unreachable

all: main vec allocator
	cc ${FLAGS} main.o vec.o allocator.o

main: main.c
	cc ${FLAGS} -c main.c -o main.o

allocator: ../../allocator/src/*
	cc ${FLAGS} -c ../../allocator/src/* -o allocator.o

vec: ../../vec/src/*
	cc ${FLAGS} -c ../../vec/src/* -o vec.o

clean:
	rm -f *.out *.o
