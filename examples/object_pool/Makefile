FLAGS := -g -Wall -Werror -Wextra -fsanitize=address,leak,undefined,unreachable

all: main allocator object_pool
	cc ${FLAGS} main.o allocator.o object_pool.o

object_pool:
	cc ${FLAGS} -c ../../object_pool/src/* -o object_pool.o

allocator:
	cc ${FLAGS} -c ../../allocator/src/* -o allocator.o

main:
	cc ${FLAGS} -c main.c -o main.o

clean:
	rm -f *.o *.out
