#include <stdlib.h>

#include "../../macro_util/macro_util.h"
#include "../allocator.h"

Error *allocator_global_error_out_of_memory = &ERROR_MAKE("out of memory");

__attribute__((used))
static void *global_alloc(void *ctx, usize num_bytes, Error **error) {
  UNUSED(ctx);

  void *c = malloc(num_bytes);
  if (UNLIKELY(error && !c)) {
    *error = allocator_global_error_out_of_memory;
  }
  return c;
}

__attribute__((used))
static void *global_realloc(void *ctx, void *chunk, usize num_bytes,
                            Error **error) {
  UNUSED(ctx);

  void *c = realloc(chunk, num_bytes);
  if (UNLIKELY(error && !c)) {
    *error = allocator_global_error_out_of_memory;
  }
  return c;
}

__attribute__((used))
static void global_free(void *ctx, void *chunk, Error **error) {
  UNUSED(ctx);
  UNUSED(error);

  free(chunk);
}

Allocator *allocator_global = &(Allocator){
    .vtable =
        &(Allocator_VTable){
            .alloc = global_alloc,
            .realloc = global_realloc,
            .free = global_free,
        },
};
