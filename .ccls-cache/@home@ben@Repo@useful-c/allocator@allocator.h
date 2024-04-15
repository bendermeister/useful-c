#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include "../error/error.h"
#include "../holy_types/holy_types.h"

typedef void *(*Allocator_Alloc_Func)(void *ctx, usize num_bytes,
                                      Error **error);

typedef void (*Allocator_Free_Func)(void *ctx, void *chunk, Error **error);
typedef void *(*Allocator_Realloc_Func)(void *ctx, void *chunk, usize num_bytes,
                                        Error **error);

typedef struct Allocator_VTable Allocator_VTable;
struct Allocator_VTable {
  const Allocator_Alloc_Func alloc;
  const Allocator_Free_Func free;
  const Allocator_Realloc_Func realloc;
};

typedef struct Allocator Allocator;
struct Allocator {
  void *ctx;
  const Allocator_VTable *const vtable;
};

__attribute__((malloc, unused)) static void *
allocator_alloc(Allocator *allocator, usize num_bytes, Error **error) {
  return allocator->vtable->alloc(allocator->ctx, num_bytes, error);
}

__attribute__((malloc, unused)) static void *
allocator_realloc(Allocator *allocator, void *chunk, usize num_bytes,
                  Error **error) {
  return allocator->vtable->realloc(allocator->ctx, chunk, num_bytes, error);
}

__attribute__((unused)) static void allocator_free(Allocator *allocator,
                                                   void *chunk, Error **error) {
  return allocator->vtable->free(allocator->ctx, chunk, error);
}

extern Error *allocator_global_error_out_of_memory;
extern Allocator *allocator_global;

#endif // ALLOCATOR_H_
