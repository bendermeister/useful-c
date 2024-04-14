#include "../../allocator/allocator_interface.h"
#include "../../error/error.h"
#include "../../holy_types/holy_types.h"
#include "../../macro_util/macro_util.h"

#include <stddef.h>
#include <string.h>

// TODO wrong allocator error in debug mode

typedef struct Head Head;
struct Head {
  u64 num_bytes;
  u64 padding;
};

typedef struct Arena Arena;
struct Arena {
  usize capacity;
  usize length;
  byte buffer[];
};

typedef struct Allocator Allocator;

Error *arena_allocator_error_chunk_to_small = &ERROR_MAKE("chunk to small");
Error *arena_allocator_error_out_of_memory = &ERROR_MAKE("out of memory");

__attribute__((const)) static usize next_sixteen(const usize x) {
  return ((x - 1) | 15) + 1;
}

__attribute__((malloc)) void *arena_alloc(Arena *arena, usize num_bytes,
                                          Error **error) {

  arena->length = next_sixteen(arena->length);

  if (UNLIKELY(arena->length + 16 + num_bytes >= arena->capacity)) {
    if (error) {
      *error = arena_allocator_error_out_of_memory;
    }
    return NULL;
  }
  Head *head = (void *)&arena->buffer[arena->length];
  head->num_bytes = num_bytes;
  arena->length += 16;

  void *chunk = &arena->buffer[arena->length];
  arena->length += num_bytes;

  return chunk;
}

__attribute__((malloc)) void *arena_realloc(Arena *arena, void *chunk,
                                            usize num_bytes, Error **error) {
  // realloc(NULL, ...)
  if (UNLIKELY(!chunk)) {
    return arena_alloc(arena, num_bytes, error);
  }

  Head *head = (void *)((byte *)chunk - 16);
  // can we just resize?
  if (chunk + head->num_bytes == arena->buffer + arena->length) {
    usize new_length = arena->length + num_bytes - head->num_bytes;
    if (UNLIKELY(new_length >= arena->capacity)) {
      if (error) {
        *error = arena_allocator_error_out_of_memory;
      }
      return NULL;
    }
    head->num_bytes = num_bytes;
    arena->length = new_length;
    return chunk;
  }

  // get new chunk and copy data
  void *new_chunk = arena_alloc(arena, num_bytes, error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  memcpy(new_chunk, chunk, head->num_bytes);
  return new_chunk;
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

void arena_allocator_destroy(Allocator *arena, Error **error) {
  UNUSED(arena);
  UNUSED(error);
}
