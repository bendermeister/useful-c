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

  int *vec = NULL;
  usize length = 0;
  usize capacity = 0;

  // reserve some 50 ints worth of space
  vec = vec_reserve(allocator_global, vec, sizeof(*vec), &capacity, 50, NULL);

  // vec_more makes sure enough space is available increases the length by
  // one and returns a pointer to the last element
  for (int i = 0; i < 100; ++i) {
    vec = vec_grow_if_needed(allocator_global, vec, sizeof(*vec), length,
                             &capacity, NULL);
    vec[length++] = i;
  }

  // vec_length returns the current length of the vector
  dump_vec(length, vec);

  for (int i = 49; i >= 0; --i) {
    vec_remove(vec, sizeof(*vec), &length, i * 2);
  }

  dump_vec(length, vec);

  for (int i = 0; i < 69; ++i) {
    vec = vec_grow_if_needed(allocator_global, vec, sizeof(*vec), length, &capacity, NULL);
    *(int *)vec_insert(vec, sizeof(*vec), &length, 10) = i;
  }

  dump_vec(length, vec);

  allocator_free(allocator_global, vec, NULL);

  return 0;
}
