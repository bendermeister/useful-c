#ifndef SLICE_H_
#define SLICE_H_

#include "../allocator/allocator.h"
#include "../macro_util/macro_util.h"
#include "../vec/vec.h"

#include <stdarg.h>
#include <stddef.h>

struct Slice_Params_ {
  Error **error;
  Allocator *allocator;
};

struct Slice_Create_Params_ {
  Error **error;
  Allocator *allocator;
  usize initial_capacity;
};

struct Slice_Head_ {
  usize length;
  usize capacity;
  byte buffer[];
};

[[maybe_unused]] [[gnu::pure]] static struct Slice_Head_ *
slice_get_head_(void *v) {
  return (struct Slice_Head_ *)((byte *)v - sizeof(struct Slice_Head_));
}

#define slice_create(TYPE, ...)                                                \
  ({                                                                           \
    usize object_size = sizeof(TYPE);                                          \
    struct Slice_Create_Params_ params = {__VA_ARGS__};                        \
    if (params.initial_capacity == 0) {                                        \
      params.initial_capacity = 16;                                            \
    }                                                                          \
    if (params.allocator == NULL) {                                            \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    (TYPE *)slice_create_(object_size, params);                                \
  })

[[maybe_unused]] [[nodiscard]] [[gnu::malloc]] static void *
slice_create_(usize object_size, struct Slice_Create_Params_ params) {

  struct Slice_Head_ *head = (struct Slice_Head_ *)allocator_alloc(
      params.allocator, sizeof(struct Slice_Head_) + object_size * params.initial_capacity, params.error);

  if (UNLIKELY(params.error && *params.error)) {
    return NULL;
  }

  *head = (struct Slice_Head_){.capacity = params.initial_capacity};
  return head->buffer;
}

#endif // SLICE_H_
