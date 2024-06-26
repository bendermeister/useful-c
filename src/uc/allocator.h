#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <uc/error.h>
#include <uc/macro_util.h>
#include <uc/types.h>

#include <errno.h>
#include <stdlib.h>

typedef void Allocator;

typedef void *(*allocator_alloc_f)(Allocator *allocator, usize num_bytes,
                                   Error *error);

typedef void *(*allocator_realloc_f)(Allocator *allocator, void *chunk,
                                     usize num_bytes, Error *error);

typedef void (*allocator_free_f)(Allocator *allocator, void *chunk);

typedef struct AllocatorVTable AllocatorVTable;
struct AllocatorVTable {
  allocator_free_f free;
  allocator_alloc_f alloc;
  allocator_realloc_f realloc;
};

typedef struct AllocatorInternal AllocatorInternal;
struct AllocatorInternal {
  AllocatorVTable *vtable;
};

static void *allocator_internal_global_alloc(Allocator *allocator,
                                             usize num_bytes, Error *error) {
  UNUSED(allocator);
  void *p = malloc(num_bytes);
  if (UNLIKELY(error && !p)) {
    *error = ENOMEM;
  }
  return p;
}

static void *allocator_internal_global_realloc(Allocator *allocator,
                                               void *chunk, usize num_bytes,
                                               Error *error) {
  UNUSED(allocator);
  void *p = realloc(chunk, num_bytes);
  if (UNLIKELY(error && !p)) {
    *error = ENOMEM;
  }
  return p;
}

static void allocator_internal_global_free(Allocator *allocator, void *chunk) {
  UNUSED(allocator);
  free(chunk);
}

static AllocatorVTable allocator_internal_global_vtable = {
    .free = allocator_internal_global_free,
    .alloc = allocator_internal_global_alloc,
    .realloc = allocator_internal_global_realloc,
};

static AllocatorInternal allocator_internal_global = {
    .vtable = &allocator_internal_global_vtable,
};

static Allocator *allocator_global = &allocator_internal_global;

static void allocator_free(Allocator *allocator, void *chunk) {
  AllocatorInternal *a = allocator;
  a->vtable->free(a, chunk);
}

static void *allocator_alloc(Allocator *allocator, usize num_bytes,
                             Error *error) {
  AllocatorInternal *a = allocator;
  return a->vtable->alloc(a, num_bytes, error);
}

static void *allocator_realloc(Allocator *allocator, void *chunk,
                               usize num_bytes, Error *error) {
  AllocatorInternal *a = allocator;
  return a->vtable->realloc(a, chunk, num_bytes, error);
}

// NOTE: this is stupid but we have to silence unused warnings
static void allocator_dummy_callee__(void);
static void allocator_dummy_caller__(void) {
  allocator_free(allocator_global, NULL);
  allocator_alloc(allocator_global, 0, NULL);
  allocator_realloc(allocator_global, NULL, 0, NULL);
  allocator_dummy_callee__();
}
static void allocator_dummy_callee__(void) { allocator_dummy_caller__(); }

#endif // ALLOCATOR_H_
