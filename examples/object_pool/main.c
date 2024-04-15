#include "../../allocator/allocator.h"
#include "../../holy_types/holy_types.h"
#include "../../macro_util/macro_util.h"
#include "../../object_pool/object_pool.h"

#include <stdio.h>

struct Vec3 {
  u32 x;
  u32 y;
  u32 z;
};

void panic(Error *error) {
  fprintf(stderr, "%s\n", error->message);
  TRAP();
}

void print_vec(struct Vec3 *v) {
  if (!v) {
    return;
  }
  printf("[%u, %u, %u]\n", v->x, v->y, v->z);
}

int main(void) {
  // Get global allocator interface. This is just a malloc wrapper
  // Error channel
  Error *error = NULL;

  // Create a new Object Pool
  Object_Pool *pool =
      object_pool_create(allocator_global, sizeof(struct Vec3), 8, &error);

  // check for errors
  if (error) {
    // handle errors
    panic(error);
  }

  static struct Vec3 *arr[1000] = {};

  for (usize i = 0; i < 100; ++i) {

    // allocate a struct Vec3 from the pool
    // again pass the error pointer to catch all errors
    struct Vec3 *v = object_pool_alloc(pool, &error);
    if (error) {
      panic(error);
    }

    *v = (struct Vec3){69, 69, 69};
    arr[i] = v;
  }

  for (usize i = 0; i < 50; ++i) {

    // deallocate a struct Vec3
    object_pool_dealloc(pool, arr[i], &error);
    if (error) {
      panic(error);
    }
  }

  for (usize i = 0; i < 100; ++i) {
    // You can also ignore error values if you are sure a function will not
    // error.
    struct Vec3 *v = object_pool_alloc(pool, NULL);
    *v = (struct Vec3){69, 69, 69};
    arr[i] = v;
  }

  for (usize i = 0; i < 1000; ++i) {
    print_vec(arr[i]);
  }

  object_pool_destroy(pool, NULL);

  printf("Hello World!\n");
  return 0;
}
