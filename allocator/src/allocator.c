#include <stdlib.h>

#include "../../macro_util/macro_util.h"
#include "../allocator_interface.h"

typedef struct Allocator Allocator;

Error *allocator_global_error_out_of_memory = &ERROR_MAKE("out of memory");

void *allocator_alloc(Allocator *interface, usize num_bytes, Error **error) {
  return interface->alloc(interface->ctx, num_bytes, error);
}

void *allocator_realloc(Allocator *interface, void *chunk, usize num_bytes,
                        Error **error) {
  return interface->realloc(interface->ctx, chunk, num_bytes, error);
}

void allocator_free(Allocator *interface, void *chunk, Error **error) {
  return interface->free(interface->ctx, chunk, error);
}

static void *global_alloc(void *ctx, usize num_bytes, Error **error) {
  UNUSED(ctx);

  void *c = malloc(num_bytes);
  if (UNLIKELY(error && !c)) {
    *error = allocator_global_error_out_of_memory;
  }
  return c;
}

static void *global_realloc(void *ctx, void *chunk, usize num_bytes,
                            Error **error) {
  UNUSED(ctx);

  void *c = realloc(chunk, num_bytes);
  if (UNLIKELY(error && !c)) {
    *error = allocator_global_error_out_of_memory;
  }
  return c;
}

static void global_free(void *ctx, void *chunk, Error **error) {
  UNUSED(ctx);
  UNUSED(error);

  free(chunk);
}

static Allocator global_allocator = {
    .alloc = global_alloc,
    .realloc = global_realloc,
    .free = global_free,
};

Allocator *allocator_get_global(void) { return &global_allocator; }
