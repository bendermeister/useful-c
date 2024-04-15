#include <stdio.h>
#include <stdlib.h>

#include "../../arena_allocator/arena_allocator.h"
#include "../../vec/vec.h"

static byte chunk[1024] = {0};

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

  Allocator *arena = arena_allocator_create(chunk, sizeof(chunk), &error);
  if (error) {
    fprintf(stderr, "Error: %s", error->message);
    abort();
  }

  int *vec = vec_create(arena, int, 8, &error);
  if (error) {
    fprintf(stderr, "Error: %s", error->message);
    abort();
  }

  for (int i = 0; 1; ++i) {
    int *more = vec_more(&vec, arena, &error);
    if (error) {
      break;
    }
    *more = i;
  }
  error = NULL;

  dump_vec(vec_length(&vec), vec);

  printf("Hello World!\n");
  return 0;
}
