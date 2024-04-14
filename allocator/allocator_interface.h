#ifndef ALLOCATOR_INTERFACE_H_
#define ALLOCATOR_INTERFACE_H_

#include "../error/error.h"
#include "../holy_types/holy_types.h"

struct Allocator {
  void *ctx;
  void *(*alloc)(void *ctx, usize num_bytes, Error **error);
  void (*free)(void *ctx, void *chunk, Error **error);
  void *(*realloc)(void *ctx, void *chunk, usize num_bytes, Error **error);
};

#endif // ALLOCATOR_INTERFACE_H_
