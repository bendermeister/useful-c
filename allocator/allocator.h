#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include "../error/error.h"
#include "../holy_types/holy_types.h"

typedef void Allocator;

void *allocator_alloc(Allocator *interface, usize num_bytes, Error **error);

void *allocator_realloc(Allocator *interface, void *chunk, usize num_bytes,
                        Error **error);

void allocator_free(Allocator *interface, void *chunk, Error **error);

Allocator *allocator_get_global(void);

extern Error *allocator_global_error_out_of_memory;

#endif // ALLOCATOR_H_
