#ifndef VEC_H_
#define VEC_H_

#include "../allocator/allocator.h"
#include "../error/error.h"
#include "../holy_types/holy_types.h"

void *vec_grow_if_needed(Allocator *allocator, void *vec, usize object_size,
                         usize length, usize *capacity, Error **error);

void *vec_reserve(Allocator *allocator, void *vec, usize object_size,
                  usize *capacity, usize to_reserve, Error **error);

void *vec_shrink(Allocator *allocator, void *vec, usize object_size,
                 usize length, usize *capacity, Error **error);

void vec_remove(void *vec, usize object_size, usize *length, usize index);
void vec_insert(void *vec, usize object_size, usize *length, usize index);

#endif // VEC_H_
