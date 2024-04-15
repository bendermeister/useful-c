#include "../vec.h"
#include "../../macro_util/macro_util.h"

#include <stddef.h>
#include <string.h>

typedef struct Head Head;
struct Head {
  usize length;
  usize capacity;
  byte buffer[];
};

static Head *get_head(void *v) { return ((Head *)v) - 1; }

__attribute__((malloc)) void *vec_create_impl(Allocator *allocator,
                                              usize object_size,
                                              usize initial_capacity,
                                              Error **error) {

  if (initial_capacity == 0) {
    initial_capacity = 4;
  }

  Head *vec = allocator_alloc(
      allocator, object_size * initial_capacity + sizeof(*vec), error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  vec->capacity = initial_capacity;
  vec->length = 0;
  return vec->buffer;
}

static void vec_realloc(void **vec, Allocator *allocator, usize object_size,
                        usize new_capacity, Error **error) {
  Head *head = get_head(*vec);

  head = allocator_realloc(allocator, head,
                           sizeof(*head) + object_size * new_capacity, error);
  if (UNLIKELY(error && *error)) {
    return;
  }
  head->capacity = new_capacity;
  *vec = head + 1;
}

void *vec_more_impl(void **vec, Allocator *allocator, usize object_size,
                    Error **error) {
  Head *head = get_head(*vec);
  if (head->length >= head->capacity) {
    vec_realloc(vec, allocator, object_size, head->capacity << 1, error);
    if (UNLIKELY(error && *error)) {
      return NULL;
    }
    head = get_head(*vec);
  }
  void *p = &head->buffer[head->length * object_size];
  head->length += 1;
  return p;
}

usize vec_length_impl(void **vec) { return get_head(*vec)->length; }

void vec_reserve_impl(void **vec, Allocator *allocator, usize object_size,
                      usize to_reserve, Error **error) {
  Head *head = get_head(*vec);
  if (head->capacity >= to_reserve) {
    return;
  }
  vec_realloc(vec, allocator, object_size, to_reserve, error);
}

void *vec_insert_impl(void **vec, Allocator *allocator, usize index,
                      usize object_size, Error **error) {
  Head *head = get_head(*vec);
  head->length += 1;

  if (head->length >= head->capacity) {
    vec_realloc(vec, allocator, object_size, head->capacity << 1, error);
    if (UNLIKELY(error && *error)) {
      return NULL;
    }
    head = get_head(*vec);
  }

  (void)memmove(&head->buffer[(index + 1) * object_size],
                &head->buffer[index * object_size],
                (head->length - index) * object_size);
  return &head->buffer[index * object_size];
}

void vec_shrink_impl(void **vec, Allocator *allocator, usize object_size,
                     Error **error) {
  vec_realloc(vec, allocator, object_size, get_head(*vec)->length, error);
}

void vec_destroy_impl(void **vec, Allocator *allocator, Error **error) {
  allocator_free(allocator, get_head(*vec), error);
}

void vec_remove_impl(void **vec, usize index, usize object_size) {
  Head *head = get_head(*vec);

  head->length -= 1;

  if (head->length == index) {
    return;
  }

  (void)memmove(&head->buffer[index * object_size],
                &head->buffer[(index + 1) * object_size],
                object_size * (head->length - index));
}
