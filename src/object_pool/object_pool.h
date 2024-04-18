#ifndef OBJECT_POOL_H_
#define OBJECT_POOL_H_

#include "../allocator/allocator.h"
#include "../error/error.h"
#include "../holy_types/holy_types.h"

typedef void Object_Pool;

Object_Pool *object_pool_create(Allocator *allocator, usize object_size,
                                usize initial_capacity, Error **error);

void object_pool_destroy(Object_Pool *object_pool, Error **error);

void *object_pool_alloc(Object_Pool *pool, Error **error);
void object_pool_dealloc(Object_Pool *pool, void *object, Error **error);

#endif // OBJECT_POOL_H_
