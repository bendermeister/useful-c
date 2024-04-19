FLAGS := -std=c99 -Wall -Wextra -pedantic
DEBUG_FLAGS := ${FLAGS} -g -fsanitize=address,leak,undefined,unreachable -DDEBUG

all: test example
	echo "Useful C"

TEST := test/table.out

EXAMPLE := example/error/error.out

test: ${TEST}

example: ${EXAMPLE}

%.out: %.c
	${CC} ${DEBUG_FLAGS} $< -o $@

clean:
	rm -f ./*/*.o ./*/*.out
