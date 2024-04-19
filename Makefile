FLAGS := -std=c99 -Wall -Wextra -pedantic
DEBUG_FLAGS := ${FLAGS} -g -fsanitize=address,leak,undefined,unreachable

all:
	echo "Useful C"

TEST := test/table.out

test: ${TEST}

test/%.out: test/%.c
	${CC} ${DEBUG_FLAGS} $< -o $@

clean:
	rm -f ./*/*.o ./*/*.out
