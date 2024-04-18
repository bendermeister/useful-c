#include "../vec.h"
#include "../../macro_util/macro_util.h"

#include <string.h>

static void *vec_realloc(Allocator *allocator, void *vec, usize object_size,
                         usize *capacity, usize new_capacity, Error **error) {
  void *new_vec =
      allocator_realloc(allocator, vec, object_size * new_capacity, error);
  if (UNLIKELY(error && *error)) {
    return vec;
  }
  *capacity = new_capacity;
  return new_vec;
}

void *vec_grow_if_needed(Allocator *allocator, void *vec, usize object_size,
                         usize length, usize *capacity, Error **error) {
  if (LIKELY(length < *capacity)) {
    return vec;
  }

  usize new_capacity;

  if (UNLIKELY(*capacity == 0)) {
    new_capacity = 16;
  } else {
    new_capacity = *capacity << 1;
  }
  return vec_realloc(allocator, vec, object_size, capacity, new_capacity,
                     error);
}

void *vec_reserve(Allocator *allocator, void *vec, usize object_size,
                  usize *capacity, usize to_reserve, Error **error) {
  if (*capacity >= to_reserve) {
    return vec;
  }
  return vec_realloc(allocator, vec, object_size, capacity, to_reserve, error);
}

void *vec_shrink(Allocator *allocator, void *vec, usize object_size,
                 usize length, usize *capacity, Error **error) {
  if (UNLIKELY(length == *capacity)) {
    return vec;
  }
  return vec_realloc(allocator, vec, object_size, capacity, length, error);
}

void vec_remove(void *vec, usize object_size, usize *length, usize index) {
  byte *dest = (byte *)vec + index * object_size;
  byte *src = dest += object_size;
  usize n = (*length - 1 - index) * object_size;
  *length -= 1;
  memmove(dest, src, n);
}

void *vec_insert(void *vec, usize object_size, usize *length, usize index) {
  byte *src = (byte *)vec + index * object_size;
  byte *dest = src + object_size;
  usize n = (*length - index) * object_size;
  memmove(dest, src, n);
  *length += 1;
  return src;
}
