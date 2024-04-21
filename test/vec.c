#include <uc/allocator.h>
#include <uc/builtin.h>
#include <uc/error.h>
#include <uc/vec.h>

#include "test.h"

static void unwrap(Error error) {
  if (error) {
    builtin_trap();
  }
}

static void test__push_pop(void) {
  Error error = 0;
  Vec(int) vec;
  vec_init(&vec, sizeof(int), 8, allocator_global, &error);
  unwrap(error);

  for (int i = 0; i < 1000; ++i) {
    TEST_INT(vec.length, i);

    vec_push(&vec, sizeof(int), &i, allocator_global, &error);
    unwrap(error);

    TEST_INT(vec.length, i + 1);
  }

  for (int i = 0; i < 1000; ++i) {
    TEST_INT(vec.element[i], i);
  }

  for (int i = 999; i >= 0; --i) {
    int dest = -1;
    TEST_INT(vec.length, i + 1);
    vec_pop(&vec, sizeof(int), &dest);
    TEST_INT(dest, i);
    TEST_INT(vec.length, i);
  }

  vec_deinit(&vec, sizeof(int), allocator_global);
}

int main(void) {
  test__push_pop();
  TEST_OVERVIEW();
  return 0;
}
