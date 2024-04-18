#include "../../allocator/allocator.h"
#include "../../error/error.h"
#include "../../holy_types/holy_types.h"
#include "../../macro_util/macro_util.h"

typedef struct Object_Pool Object_Pool;
struct Object_Pool {
  byte *object_list;
  byte **object_buffer;
  Allocator *allocator;

  u32 current_buffer;
  u32 buffer_index;
  u32 buffer_capacity;

  u32 object_size;
};

static void object_buffer_add(Object_Pool *pool, u32 new_buffer_capacity,
                              Error **error) {

  u32 new_current_buffer = pool->current_buffer + 1;
  u32 new_buffer_index = 0;

  void *new_object_buffer = allocator_realloc(
      pool->allocator, pool->object_buffer,
      sizeof(*pool->object_buffer) * (new_current_buffer + 1), error);

  if (UNLIKELY(error && *error)) {
    return;
  }

  pool->object_buffer = new_object_buffer;

  pool->object_buffer[new_current_buffer] = allocator_alloc(
      pool->allocator, pool->object_size * new_buffer_capacity, error);

  if (UNLIKELY(error && *error)) {
    return;
  }

  pool->current_buffer = new_current_buffer;
  pool->buffer_index = new_buffer_index;
  pool->buffer_capacity = new_buffer_capacity;
}

Object_Pool *object_pool_create(Allocator *allocator, usize object_size,
                                usize initial_capacity, Error **error) {

  if (object_size < sizeof(void *)) {
    object_size = sizeof(void *);
  }

  Object_Pool *pool = allocator_alloc(allocator, sizeof(*pool), error);
  if (UNLIKELY(error && *error)) {
    return 0;
  }

  *pool = (Object_Pool){
      .allocator = allocator,
      .buffer_capacity = initial_capacity,
      .object_size = object_size,
      .current_buffer = (u32)-1, // this is a bit dirty
  };

  object_buffer_add(pool, initial_capacity, error);
  if (UNLIKELY(error && *error)) {
    // TODO how to handle error in free?
    allocator_free(pool->allocator, pool, 0);
    return 0;
  }
  return pool;
}

void object_pool_destroy(Object_Pool *pool, Error **error) {
  for (usize i = 0; i <= pool->current_buffer; ++i) {
    allocator_free(pool->allocator, pool->object_buffer[i], error);
    if (UNLIKELY(error && *error)) {
      return;
    }
  }

  allocator_free(pool->allocator, pool->object_buffer, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  allocator_free(pool->allocator, pool, error);
  if (UNLIKELY(error && *error)) {
    return;
  }
}

static void *buffer_pop(Object_Pool *pool, Error **error) {
  if (pool->buffer_index >= pool->buffer_capacity) {
    object_buffer_add(pool, pool->buffer_capacity << 1, error);
    if (UNLIKELY(error && *error)) {
      return 0;
    }
  }
  void *p = pool->object_buffer[pool->current_buffer] +
            pool->buffer_index * pool->object_size;
  pool->buffer_index += 1;
  return p;
}

static void *list_pop(Object_Pool *pool) {
  if (!pool->object_list) {
    return 0;
  }
  void *p = pool->object_list;
  pool->object_list = *(void **)pool->object_list;
  return p;
}

static void list_push(Object_Pool *pool, void *object) {
  *(void **)object = pool->object_list;
  pool->object_list = object;
}

void *object_pool_alloc(Object_Pool *pool, Error **error) {
  void *p = list_pop(pool);
  if (p) {
    return p;
  }
  p = buffer_pop(pool, error);
  if (UNLIKELY(error && *error)) {
    return 0;
  }
  return p;
}

void object_pool_dealloc(Object_Pool *pool, void *object, Error **error) {
  UNUSED(error);
  list_push(pool, object);
}
