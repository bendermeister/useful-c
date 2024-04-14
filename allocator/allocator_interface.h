#ifndef ALLOCATOR_INTERFACE_H_
#define ALLOCATOR_INTERFACE_H_

#include "../error/error.h"
#include "../holy_types/holy_types.h"

typedef void *(*Allocator_Alloc_Func)(void *ctx, usize num_bytes,
                                      Error **error);

typedef void (*Allocator_Free_Func)(void *ctx, void *chunk, Error **error);
typedef void *(*Allocator_Realloc_Func)(void *ctx, void *chunk, usize num_bytes,
                                        Error **error);

struct Allocator {
  void *ctx;
  Allocator_Alloc_Func alloc;
  Allocator_Free_Func free;
  Allocator_Realloc_Func realloc;
};

#endif // ALLOCATOR_INTERFACE_H_
