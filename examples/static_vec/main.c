#include <stdio.h>
#include <stdlib.h>

#include "../../arena_allocator/arena_allocator.h"
#include "../../vec/vec.h"

static byte chunk[4096] = {0};

static void dump_vec(usize len, int vec[len]) {
  puts("=====================================================================");
  for (usize i = 1; i < len + 1; ++i) {
    printf("% 4d ", vec[i - 1]);
    if (i % 10 == 0) {
      putc('\n', stdout);
    }
  }
  putc('\n', stdout);
  puts("=====================================================================");
  putc('\n', stdout);
}

int main(void) {
  Error *error = NULL;

  Allocator arena_ = arena_allocator_create(chunk, sizeof(chunk), &error);
  if (error) {
    fprintf(stderr, "Error: %s", error->message);
    abort();
  }
  Allocator *arena = &arena_;

  int *vec = NULL;
  usize length = 0;
  usize capacity = 0;

  vec = vec_reserve(arena, vec, sizeof(*vec), &capacity, 12, &error);
  if (error) {
    fprintf(stderr, "Error: %s", error->message);
    abort();
  }

  for (int i = 0; 1; ++i) {
    vec =
        vec_grow_if_needed(arena, vec, sizeof(*vec), length, &capacity, &error);
    if (error) {
      break;
    }
    length += 1;
    vec[length] = i;
  }
  error = NULL;

  dump_vec(length, vec);

  allocator_free(arena, vec, NULL);

  printf("Hello World!\n");
  return 0;
}
