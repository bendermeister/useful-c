#include "../../allocator/allocator_interface.h"
#include "../../error/error.h"
#include "../../holy_types/holy_types.h"
#include "../../macro_util/macro_util.h"

#include <stddef.h>

typedef struct Arena Arena;
struct Arena {
  usize capacity;
  usize length;
  byte buffer[];
};

typedef struct Allocator Allocator;

Error *arena_allocator_error_chunk_to_small = &ERROR_MAKE("chunk to small");
Error *arena_allocator_error_out_of_memory = &ERROR_MAKE("out of memory");

void *arena_alloc(Arena *arena, usize num_bytes, Error **error) { return NULL; }

void *arena_realloc(Arena *arena, void *chunk, usize num_bytes, Error **error) {
  return NULL;
}

void arena_free(Arena *arena, void *chunk) {
  UNUSED(arena);
  UNUSED(chunk);
}

Allocator *arena_allocator_create(void *chunk, usize chunk_size,
                                  Error **error) {
  if (UNLIKELY(error && chunk_size < sizeof(Arena) + sizeof(Allocator))) {
    *error = arena_allocator_error_chunk_to_small;
    return NULL;
  }
  Arena *arena = chunk;
  arena->capacity = chunk_size;
  arena->length = sizeof(Allocator);

  Allocator *allocator = chunk;
  allocator->ctx = arena;
  allocator->alloc = (Allocator_Alloc_Func)arena_alloc;
  allocator->realloc = (Allocator_Realloc_Func)arena_realloc;
  allocator->free = (Allocator_Free_Func)arena_free;
  return allocator;
}

void arena_allocator_destroy(Allocator *arena, Error **error);
