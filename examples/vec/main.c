#include "../../allocator/allocator.h"
#include "../../vec/vec.h"
#include <stdio.h>

static void dump_vec(usize len, int vec[len]) {
  for (usize i = 0; i < len; ++i) {
    printf("% 4d ", vec[i]);
    if (i % 10 == 0) {
      putc('\n', stdout);
    }
  }
  putc('\n', stdout);
  putc('\n', stdout);
}

int main(void) {
  printf("Hello World!\n");
  int *vec = vec_create(allocator_global, int, 8, NULL);

  // reserve some 50 ints worth of space
  vec_reserve(&vec, allocator_global, 50, NULL);

  // vec_more makes sure enough space is available increases the length by one
  // and returns a pointer to the last element
  for (int i = 0; i < 100; ++i) {
    *vec_more(&vec, allocator_global, NULL) = i;
  }

  // vec_length returns the current length of the vector
  dump_vec(vec_length(&vec), vec);

  for (int i = 49; i >= 0; --i) {
    vec_remove(&vec, i * 2);
  }

  dump_vec(vec_length(&vec), vec);

  for (int i = 0; i < 69; ++i) {
    *vec_insert(&vec, allocator_global, 10, NULL) = i;
  }

  dump_vec(vec_length(&vec), vec);

  vec_destroy(&vec, allocator_global, NULL);

  return 0;
}
