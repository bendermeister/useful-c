#ifndef CVEC_H_
#define CVEC_H_

#include "allocator.h"
#include "builtin.h"
#include "debug_check.h"
#include "macro_util.h"

// TODO: documentation
typedef void (*cvec_move_f)(void *dest, void *src, void *ctx);
typedef void (*cvec_destroy_f)(void *element, void *ctx);

typedef struct CVecVTable CVecVTable;
struct CVecVTable {
  cvec_move_f move;
  cvec_destroy_f destroy;
  usize element_size;
  void *ctx;
};

typedef void CVec;

#define CVec(TYPE)                                                             \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
  }

static void cvec_init(CVec *cvec_, const CVecVTable *vtable,
                      usize initial_capacity, Allocator *allocator,
                      Error *error) {
  debug_check(cvec_);
  debug_check(allocator);
  debug_check(initial_capacity > 0);
  debug_check(vtable);

  CVec(byte) *cvec = cvec_;
  cvec->end = initial_capacity;
  cvec->length = 0;

  usize chunk_size = vtable->element_size * cvec->end;
  cvec->element = allocator_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }
}

static void cvec_clear(CVec *cvec, const CVecVTable *vtable);

static void cvec_deinit(CVec *cvec_, const CVecVTable *vtable,
                        Allocator *allocator) {
  debug_check(cvec_);
  debug_check(allocator);

  cvec_clear(cvec_, vtable);

  CVec(byte) *cvec = cvec_;
  allocator_free(allocator, cvec->element);
}

static void cvec_internal_realloc(CVec *cvec_, const CVecVTable *vtable,
                                  usize new_end, Allocator *allocator,
                                  Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(new_end > 0);
  debug_check(allocator);

  CVec(byte) *cvec = cvec_;

  usize chunk_size = vtable->element_size * new_end;

  void *p = allocator_realloc(allocator, cvec->element, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  cvec->element = p;
  cvec->end = new_end;
}

static void *cvec_more(CVec *cvec_, const CVecVTable *vtable,
                       Allocator *allocator, Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(allocator);

  CVec(byte) *cvec = cvec_;
  if (cvec->length >= cvec->end) {
    cvec_internal_realloc(cvec, vtable, cvec->end << 1, allocator, error);
    if (UNLIKELY(error && *error)) {
      return NULL;
    }
  }

  void *p = cvec->element + vtable->element_size * cvec->length;
  cvec->length += 1;
  return p;
}

static void cvec_push(CVec *cvec_, const CVecVTable *vtable, void *element,
                      Allocator *allocator, Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(element);
  debug_check(allocator);

  void *dest = cvec_more(cvec_, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  vtable->move(dest, element, vtable->ctx);
}

static void cvec_pop(CVec *cvec_, const CVecVTable *vtable, void *dest) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(dest);

  CVec(byte) *cvec = cvec_;
  debug_check(cvec->length > 0);

  cvec->length -= 1;
  void *src = cvec->element + vtable->element_size * cvec->length;

  vtable->move(dest, src, vtable->ctx);
}

static void cvec_remove(CVec *cvec_, const CVecVTable *vtable, usize index) {
  debug_check(cvec_);
  debug_check(vtable);

  CVec(byte) *cvec = cvec_;
  debug_check(cvec->length > index);

  void *dest = cvec->element + vtable->element_size * index;
  vtable->destroy(dest, vtable->ctx);
  cvec->length -= 1;
  void *src = (byte *)dest + vtable->element_size;
  usize num_bytes = (cvec->length - index) * vtable->element_size;
  builtin_memcpy(dest, src, num_bytes);
}

static void cvec_insert(CVec *cvec_, const CVecVTable *vtable, usize index,
                        void *element, Allocator *allocator, Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(allocator);
  debug_check(element);

  CVec(byte) *cvec = cvec_;
  if (UNLIKELY(cvec->length >= cvec->end)) {
    cvec_internal_realloc(cvec, vtable, cvec->end << 1, allocator, error);
    if (UNLIKELY(error && *error)) {
      return;
    }
  }

  debug_check(cvec->length >= index);

  byte *src = cvec->element + vtable->element_size * index;
  byte *dest = src + vtable->element_size;
  usize num_bytes = (cvec->length - index) * vtable->element_size;
  builtin_memcpy(dest, src, num_bytes);

  vtable->move(src, element, vtable->ctx);
}

static void cvec_clear(CVec *cvec_, const CVecVTable *vtable) {
  debug_check(cvec_);
  debug_check(vtable);

  CVec(byte) *cvec = cvec_;

  usize end = cvec->length * vtable->element_size;
  for (usize i = 0; i < end; i += vtable->element_size) {
    vtable->destroy(cvec->element + i, vtable->ctx);
  }
  cvec->length = 0;
}

static void cvec_reserve(CVec *cvec_, const CVecVTable *vtable, usize capacity,
                         Allocator *allocator, Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(allocator);
  debug_check(capacity > 0);

  CVec(byte) *cvec = cvec_;
  if (cvec->end >= capacity) {
    return;
  }
  cvec_internal_realloc(cvec, vtable, capacity, allocator, error);
}

static void cvec_shrink(CVec *cvec_, const CVecVTable *vtable,
                        Allocator *allocator, Error *error) {
  debug_check(cvec_);
  debug_check(vtable);
  debug_check(allocator);

  CVec(byte) *cvec = cvec_;

  cvec_internal_realloc(cvec_, vtable, cvec->length, allocator, error);
}

#endif // CVEC_H_
