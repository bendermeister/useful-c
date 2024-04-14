#ifndef ARENA_ALLOCATOR_H_
#define ARENA_ALLOCATOR_H_

#include "../allocator/allocator.h"

Allocator *arena_allocator_create(void *chunk, usize chunk_size, Error **error);
void arena_allocator_destroy(Allocator *arena, Error **error);

extern Error *arena_allocator_error_chunk_to_small;
extern Error *arena_allocator_error_out_of_memory;

#endif // ARENA_ALLOCATOR_H_
