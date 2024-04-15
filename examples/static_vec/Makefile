FLAGS := -g -Wall -Wextra -std=c2x -fsanitize=address,leak,undefined,unreachable
all: main allocator vec arena_allocator
	cc ${FLAGS} main.o allocator.o vec.o arena_allocator.o

main: main.c
	cc ${FLAGS} -c main.c -o main.o

allocator: ../../allocator/src/*
	cc ${FLAGS} -c ../../allocator/src/* -o allocator.o

arena_allocator: ../../arena_allocator/src/*
	cc ${FLAGS} -c ../../arena_allocator/src/* -o arena_allocator.o

vec: ../../vec/src/*
	cc ${FLAGS} -c ../../vec/src/* -o vec.o

clean:
	rm -f *.out *.o
