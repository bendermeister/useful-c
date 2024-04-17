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
      params.allocator,
      sizeof(struct Slice_Head_) + object_size * params.initial_capacity,
      params.error);

  if (UNLIKELY(params.error && *params.error)) {
    return NULL;
  }

  *head = (struct Slice_Head_){.capacity = params.initial_capacity};
  return head->buffer;
}

#define slice_destroy(SLICE, ...)                                              \
  ({                                                                           \
    struct Slice_Params_ params = {__VA_ARGS__};                               \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    slice_destroy_(SLICE, params);                                             \
  })
[[maybe_unused]] static void slice_destroy_(void *slice,
                                            struct Slice_Params_ params) {
  if (!slice) {
    return;
  }
  allocator_free(params.allocator, slice_get_head_(slice), params.error);
}

[[maybe_unused]] [[gnu::malloc]] [[nodiscard]] static struct Slice_Head_ *
slice_realloc_(struct Slice_Head_ *head, usize object_size, usize new_capacity,
               struct Slice_Params_ params) {
  head = (struct Slice_Head_ *)allocator_realloc(
      params.allocator, head, sizeof(*head) + object_size * new_capacity,
      params.error);
  if (UNLIKELY(params.error && *params.error)) {
    return NULL;
  }
  head->capacity = new_capacity;
  return head;
}

#define slice_more(SLICE, ...)                                                 \
  (typeof(SLICE))({                                                            \
    void **slice = (void **)&SLICE;                                            \
    struct Slice_Params_ params = {__VA_ARGS__};                               \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    slice_more_(slice, sizeof(*SLICE), params);                                \
  })
[[maybe_unused]] [[nodiscard]] static void *
slice_more_(void **slice, usize object_size, struct Slice_Params_ params) {
  struct Slice_Head_ *head = slice_get_head_(*slice);
  if (head->length >= head->capacity) {
    head = slice_realloc_(head, object_size, head->capacity << 1, params);
    if (UNLIKELY(params.error && *params.error)) {
      return NULL;
    }
    *slice = head->buffer;
  }

  void *p = &head->buffer[head->length * object_size];
  head->length += 1;
  return p;
}

#define slice_length(SLICE) ({ slice_get_head_(SLICE)->length; })

#define slice_insert(SLICE, INDEX, ...)                                        \
  (typeof(SLICE))({                                                            \
    void **slice = (void **)&SLICE;                                            \
    struct Slice_Params_ params = {__VA_ARGS__};                               \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    slice_insert_(slice, sizeof(*SLICE), INDEX, params);                       \
  })

[[maybe_unused]] [[nodiscard]] static void *
slice_insert_(void **slice, usize object_size, usize index,
              struct Slice_Params_ params) {
  struct Slice_Head_ *head = slice_get_head_(*slice);

  if (head->length >= head->capacity) {
    head = slice_realloc_(head, object_size, head->capacity << 1, params);
    if (UNLIKELY(params.error && *params.error)) {
      return NULL;
    }
    *slice = head->buffer;
  }
  return vec_insert(head->buffer, object_size, &head->length, index);
}

#define slice_remove(SLICE, INDEX) slice_remove_(SLICE, sizeof(*SLICE), INDEX)
[[maybe_unused]] static void slice_remove_(void *slice, usize object_size,
                                           usize index) {
  struct Slice_Head_ *head = slice_get_head_(slice);
  vec_remove(slice, object_size, &head->length, index);
}

#endif // SLICE_H_
