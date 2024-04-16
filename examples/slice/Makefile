FLAGS := -g -Wall -Wextra -fsanitize=address,leak,undefined,unreachable -std=c2x

all: main allocator vec
	${CC} ${FLAGS} main.o allocator.o vec.o

main: main.c
	${CC} ${FLAGS} -c main.c -o main.o

allocator: ../../allocator/src/*
	${CC} ${FLAGS} -c ../../allocator/src/* -o allocator.o

vec: ../../vec/src/*
	${CC} ${FLAGS} -c ../../vec/src/* -o vec.o

clean:
	rm -f *.o *.out
