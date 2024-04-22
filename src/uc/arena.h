#ifndef ARENA_H_
#define ARENA_H_

#include <uc/allocator.h>
#include <uc/debug_check.h>

typedef struct Arena Arena;
struct Arena {
  const AllocatorVTable *vtable;
  byte *buffer;
  usize end;
  usize used;
};

typedef struct ArenaHead ArenaHead;
struct ArenaHead {
  u64 chunk_size;
  u64 curr_used;
  byte buffer[];
};

static u64 arena_internal_next_mult_of_16(usize x) {
  return ((x - 1) | 15) + 1;
}

static void *arena_internal_alloc(Allocator *allocator, usize chunk_size,
                                  Error *error) {
  debug_check(allocator);
  debug_check(chunk_size >
              0); // TODO: is alloc alowed to accept 0 as chunk size

  Arena *arena = allocator;
  chunk_size = arena_internal_next_mult_of_16(chunk_size);

  if (UNLIKELY(error && arena->used + 16 + chunk_size >= arena->end)) {
    *error = ENOMEM;
    return NULL;
  }
  ArenaHead *head = (ArenaHead *)(arena->buffer + arena->used);
  head->chunk_size = chunk_size;
  arena->used += 16 + chunk_size;
  head->curr_used = arena->used;
  return head->buffer;
}

static void *arena_internal_realloc(Allocator *allocator, void *chunk,
                                    usize chunk_size, Error *error) {
  debug_check(allocator);
  debug_check(chunk_size > 0); // TODO: is this right?
  debug_check(sizeof(ArenaHead) == 16);

  if (UNLIKELY(!chunk)) {
    return arena_internal_alloc(allocator, chunk_size, error);
  }

  Arena *arena = allocator;

  chunk_size = arena_internal_next_mult_of_16(chunk_size);

  ArenaHead *head = (ArenaHead *)((byte *)chunk - 16);
  if (head->curr_used == arena->used) {
    if (UNLIKELY(error &&
                 arena->used + chunk_size - head->chunk_size >= arena->end)) {
      *error = ENOMEM;
      return NULL;
    }
    arena->used += chunk_size - head->chunk_size;
    head->chunk_size = chunk_size;
    head->curr_used = arena->used;
    return head->buffer;
  }

  void *chunk_new = arena_internal_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  usize num_bytes =
      head->chunk_size < chunk_size ? head->chunk_size : chunk_size;

  (void)builtin_memcpy(chunk_new, chunk, num_bytes);
  return chunk_new;
}

static void arena_internal_free(Allocator *allocator, void *chunk) {
  debug_check(allocator);
  debug_check(sizeof(ArenaHead) == 16);

  if (!chunk) {
    return;
  }

  Arena *arena = allocator;
  ArenaHead *head = (ArenaHead *)((byte *)chunk - 16);

  if (arena->used == head->curr_used) {
    arena->used -= 16 - head->chunk_size;
  }
}

static const AllocatorVTable *arena_internal_vtable = &(AllocatorVTable){
    .alloc = arena_internal_alloc,
    .realloc = arena_internal_realloc,
    .free = arena_internal_free,
};

static void arena_init(Arena *arena, void *chunk, usize chunk_size) {
  *arena = (Arena){0};
  arena->vtable = arena_internal_vtable;
  arena->buffer = chunk;
  arena->end = chunk_size;
}

// ********************************UNUSED*WRAPPER*******************************
static void arena_unused_dummy_wrapper_(void);
static void arena_unused_dummy_wrapper__(void) {
  Arena arena;
  arena_init(&arena, NULL, 0);
  arena_unused_dummy_wrapper_();
}

static void arena_unused_dummy_wrapper_(void) {
  arena_unused_dummy_wrapper__();
}

#endif // ARENA_H_
