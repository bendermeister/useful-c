#include <stdio.h>
#include <stdlib.h>

#include <uc/debug_check.h>
#include <uc/error.h>
#include <uc/ucx.h>

enum { EDIV0 = 1 };
int average(ucx_Vec *vec_, Error *error) {
  debug_check(vec_);
  ucx_Vec(int) *vec = vec_;

  debug_check(error || vec->length);

  if (error && vec->length == 0) {
    *error = EDIV0;
    return 0;
  }

  int sum = 0;
  for (usize i = 0; i < vec->length; ++i) {
    sum += vec->element[i];
  }
  return sum / vec->length;
}

int main(void) {
  printf("Hello World!\n");

  ucx_Error error = 0;

  ucx_Vec(int) vec;
  ucx_vec_init(&vec, sizeof(int), 8, ucx_allocator_global, &error);
  if (error) {
    (void)fprintf(stderr, "Error with code: %d\n", error);
    exit(1);
  }

  for (int i = 0; i < 100; ++i) {
    *(int *)ucx_vec_more(&vec, sizeof(int), ucx_allocator_global, NULL) = i;
  }

  printf("Average of the first 100 natural numbers is: %d\n",
         average(&vec, NULL));

  ucx_vec_deinit(&vec, sizeof(int), ucx_allocator_global);

  return 0;
}
