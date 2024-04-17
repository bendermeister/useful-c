#include "../../slice/slice.h"

#include <stdio.h>

static void dump_vec(usize length, int vec[length]) {
  printf("===================================================\n");
  for (usize i = 0; i < length; ++i) {
    printf("% 4d ", vec[i]);
    if ((i + 1) % 10 == 0) {
      putc('\n', stdout);
    }
  }
  putc('\n', stdout);
  printf("===================================================\n");
  putc('\n', stdout);
}

int main(void) {
  printf("Hello World!\n");

  int *ints = slice_create(int);

  for (int i = 0; i < 100; ++i) {
    *slice_more(ints) = i;
  }

  dump_vec(slice_length(ints), ints);

  for (int i = 0; i < 100; ++i) {
    *slice_insert(ints, 50) = i;
  }

  dump_vec(slice_length(ints), ints);

  for (int i = 0; i < 100; ++i) {
    slice_remove(ints, 50);
  }

  dump_vec(slice_length(ints), ints);

  slice_destroy(ints);

  return 0;
}
