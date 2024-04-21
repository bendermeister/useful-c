#include "test.h"

#include <uc/arena.h>
#include <uc/vec.h>

static byte chunk[1024];
static usize chunk_size = sizeof(chunk);

static void test__arena(void) {
  Arena arena;
  arena_init(&arena, chunk, chunk_size);

  Error error = 0;

  Vec(int) vec;
  vec_init(&vec, sizeof(int), 1, &arena, &error);

  TEST_INT(error, 0);

  for (int i = 0; !error; ++i) {
    vec_push(&vec, sizeof(int), &i, &arena, &error);
  }

  TEST_INT(vec.length, 128);

  // TODO: how to test this further

  vec_deinit(&vec, sizeof(int), &arena);
}

int main(void) {
  test__arena();
  TEST_OVERVIEW();
  return 0;
}
