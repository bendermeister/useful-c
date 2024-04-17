FLAGS := -std=c2x -Wall -Wextra -g -fsanitize=address,leak,undefined,unreachable

all: main allocator
	${CC} ${FLAGS} main.o allocator.o

allocator: ../../allocator/src/*.c
	${CC} ${FLAGS} -c ../../allocator/src/* -o allocator.o

main: main.c
	${CC} ${FLAGS} -c main.c -o main.o

clean:
	rm -f *.o *.out
