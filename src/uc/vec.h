#ifndef VEC_H_
#define VEC_H_

#include <uc/allocator.h>
#include <uc/debug_check.h>
#include <uc/error.h>
#include <uc/types.h>

typedef void Vec;

#define Vec(TYPE)                                                              \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
  }

/***
 * @doc(function): vec_init
 * @tag: all
 *
 * @brief Initilize a vector.
 *
 * @param(vec): a valid pointer to a vec struct following the type defined by
 * `Vec(TYPE)`
 * @assert(vec): `vec != NULL`
 *
 * @param(element_size): the size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(initial_capacity): initial capacity of the vec, the vec may have an
 * initial capacity greater than the provider number but never less
 * @assert(initial_capacity): `initial_capacity > 0`
 *
 * @param(allocator): allocator used for allocating the internal element buffer
 * @assert(allocator): `allocator != NULL`
 *
 * @param(error): error pointer used in case of an error. this might be `NULl`
 *
 * @error: may set error to any error thrown by the provided allocator
 */
static void vec_init(Vec *vec, usize element_size, usize initial_capacity,
                     Allocator *allocator, Error *error);

/***
 * @doc(function): vec_init
 * @tag: all
 *
 * @brief: deinitilizes a vector initilized by `vec_init(...)`
 *
 * @param(vec): pointer to the vector which is to be deinitilized.
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size) size of the elements in the table
 * @assert(element_size > 0)
 *
 * @param(allocator): allocator used for deallocating the internal buffer.
 * @assert(allocator): `allocator != NULL`
 * @assert(allocator): allocator must be the same as the allocator used in
 * `vec_init` or at least be able to work with objects allocated by the
 * allocator used in `vec_init`
 */
static void vec_deinit(Vec *vec, usize element_size, Allocator *allocator);

/***
 * @doc(function): vec_more
 * @tag: all
 *
 * @brief: Provides a pointer for a new element in the vec
 *
 * @detailed: pushes an uninitilized element into the vector and returns a
 * pointer to it for the caller to initilize
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(allocator): allocator used for reallocing the internal array if
 * necessary
 * @assert(allocator): `allocator != NULl`
 * @assert(allocator): must be the same as in `vec_init` or be able to handle
 * data pointers return from the one used in `vec_init`
 *
 * @param(error): error pointer used to track errors which occure inside
 * `vec_more`
 *
 * @error: each error which the provided allocator may invoke
 */
static void *vec_more(Vec *vec, usize element_size, Allocator *allocator,
                      Error *error);

/***
 * @doc(function): vec_push
 * @tag: all
 *
 * @brief: pushes a new element into the vector
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(element): pointer to the element which is to be pushed into the vec
 * @assert(element): `element != NULL`
 *
 * @param(allocator): allocator used for reallocing the internal array if
 * necessary
 * @assert(allocator): `allocator != NULl`
 * @assert(allocator): must be the same as in `vec_init` or be able to handle
 * data pointers return from the one used in `vec_init`
 *
 * @param(error): error pointer used to track errors which occure inside
 * `vec_more`
 *
 * @error: each error which the provided allocator may invoke
 */
static void vec_push(Vec *vec, usize element_size, const void *element,
                     Allocator *allocator, Error *error);

/***
 * @doc(function): vec_pop
 * @tag: all
 *
 * @brief: moves the last element in the vector into `dest`
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(dest): destination pointer for the popped element. may be `NULL`
 */
static void vec_pop(Vec *vec, usize element_size, void *dest);

/***
 * @doc(function): vec_insert
 * @tag: all
 *
 * @brief: inserts an element into the vec at an specified index
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(index): index at which the element is to be inserted
 * @assert(index): `index < vec->length`
 *
 * @param(element): pointer to the element which is to be inserted into the vec
 * @assert(element): `element != NULL`
 *
 * @param(allocator): allocator used for reallocing the internal array if
 * necessary
 * @assert(allocator): `allocator != NULl`
 * @assert(allocator): must be the same as in `vec_init` or be able to handle
 * data pointers return from the one used in `vec_init`
 *
 * @param(error): error pointer used to track errors which occure inside
 * `vec_more`
 *
 * @error: each error which the provided allocator may invoke
 */
static void vec_insert(Vec *vec, usize element_size, usize index,
                       const void *element, Allocator *allocator, Error *error);

/***
 * @doc(function): vec_remove
 * @tag: all
 *
 * @brief: removes an the element at index from the table
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(index): index at which the element is to be inserted
 * @assert(index): `index < vec->length`
 */
static void vec_remove(Vec *vec, usize element_size, usize index);

/***
 * @doc(function): vec_clear
 * @tag: all
 *
 * @brief: clears the vector. sets `vec->length = 0` but will keep the reserved
 * space
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 */
static void vec_clear(Vec *vec, usize element_size);

/***
 * @doc(function): vec_reserve
 * @tag: all
 *
 * @brief: reserves space for more elements
 *
 * @detailed: reserves space for more elements, may allocate more space than
 * asked
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(new_capacity): the new capacity of the vec
 * @assert(new_capacity): `new_capacity > 0`
 *
 * @param(allocator): allocator used for reallocing the internal array if
 * necessary
 * @assert(allocator): `allocator != NULl`
 * @assert(allocator): must be the same as in `vec_init` or be able to handle
 * data pointers return from the one used in `vec_init`
 *
 * @param(error): error pointer used to track errors which occure inside
 * `vec_more`
 *
 * @error: each error which the provided allocator may invoke
 */
static void vec_reserve(Vec *vec, usize element_size, usize new_capacity,
                        Allocator *allocator, Error *error);

/***
 * @doc(function): vec_shrink
 * @tag: all
 *
 * @brief: shrinks to vector to its length
 *
 * @param(vec): vec where the element will be inserted
 * @assert(vec): `vec != NULL`
 * @assert(vec): `vec` must have been initilized with `vec_init`
 *
 * @param(element_size): size of the elements in the vec
 * @assert(element_size): `element_size > 0`
 *
 * @param(allocator): allocator used for reallocing the internal array if
 * necessary
 * @assert(allocator): `allocator != NULl`
 * @assert(allocator): must be the same as in `vec_init` or be able to handle
 * data pointers return from the one used in `vec_init`
 *
 * @param(error): error pointer used to track errors which occure inside
 * `vec_more`
 *
 * @error: each error which the provided allocator may invoke
 */
static void vec_shrink(Vec *vec, usize element_size, Allocator *allocator,
                       Error *error);

// ********************************INTERNAL***********************************

static void vec_internal_realloc(Vec *vec_, usize element_size,
                                 usize num_elements, Allocator *allocator,
                                 Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(num_elements > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;
  void *p = allocator_realloc(allocator, vec->element,
                              num_elements * element_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }
  vec->end = num_elements;
  vec->element = p;
}

static void vec_internal_grow_if_needed(Vec *vec_, usize element_size,
                                        Allocator *allocator, Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;
  if (vec->length >= vec->end) {
    vec_internal_realloc(vec, element_size, vec->end << 1, allocator, error);
  }
}

static void vec_init(Vec *vec_, usize element_size, usize initial_capacity,
                     Allocator *allocator, Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(initial_capacity > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;
  builtin_memset(vec, 0, sizeof(*vec));

  vec->end = initial_capacity;

  vec->element = allocator_alloc(allocator, vec->end * element_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }
}

static void vec_deinit(Vec *vec_, usize element_size, Allocator *allocator) {
  UNUSED(element_size);

  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;

  allocator_free(allocator, vec->element);
}

static void *vec_more(Vec *vec_, usize element_size, Allocator *allocator,
                      Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(allocator);

  vec_internal_grow_if_needed(vec_, element_size, allocator, error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  Vec(byte) *vec = vec_;

  void *p = vec->element + vec->length * element_size;
  vec->length += 1;
  return p;
}

static void vec_push(Vec *vec_, usize element_size, const void *element,
                     Allocator *allocator, Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(element);
  debug_check(allocator);

  vec_internal_grow_if_needed(vec_, element_size, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  Vec(byte) *vec = vec_;

  void *dest = vec->element + vec->length * element_size;
  builtin_memcpy(dest, element, element_size);
  vec->length += 1;
}

static void vec_pop(Vec *vec_, usize element_size, void *dest) {
  debug_check(vec_);
  debug_check(element_size > 0);

  Vec(byte) *vec = vec_;

  if (!vec->length) {
    return;
  }
  vec->length -= 1;
  if (dest) {
    void *src = vec->element + vec->length * element_size;
    builtin_memcpy(dest, src, element_size);
  }
}

static void vec_insert(Vec *vec_, usize element_size, usize index,
                       const void *element, Allocator *allocator,
                       Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(element);
  debug_check(allocator);

  Vec(byte) *vec = vec_;
  debug_check(vec->length > index);

  vec_internal_grow_if_needed(vec, element_size, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  byte *src = vec->element + index * element_size;
  byte *dest = src + element_size;
  usize num_bytes = element_size * (vec->length - index);

  (void)builtin_memmove(dest, src, num_bytes);
  vec->length += 1;
  (void)builtin_memcpy(src, element, element_size);
}

static void vec_remove(Vec *vec_, usize element_size, usize index) {
  debug_check(vec_);
  debug_check(element_size > 0);

  Vec(byte) *vec = vec_;

  debug_check(index < vec->length);

  byte *dest = vec->element + element_size * index;
  byte *src = dest + element_size;
  usize num_bytes = element_size * (vec->length - index - 1);
  builtin_memmove(dest, src, num_bytes);
  vec->length -= 1;
}

static void vec_clear(Vec *vec_, usize element_size) {
  UNUSED(element_size);
  debug_check(vec_);
  debug_check(element_size > 0);

  Vec(byte) *vec = vec_;
  vec->length = 0;
}

static void vec_reserve(Vec *vec_, usize element_size, usize new_capacity,
                        Allocator *allocator, Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(new_capacity > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;

  if (vec->end >= new_capacity) {
    return;
  }
  vec_internal_realloc(vec, element_size, new_capacity, allocator, error);
}

static void vec_shrink(Vec *vec_, usize element_size, Allocator *allocator,
                       Error *error) {
  debug_check(vec_);
  debug_check(element_size > 0);
  debug_check(allocator);

  Vec(byte) *vec = vec_;
  if (vec->length == vec->end) {
    return;
  }
  vec_internal_realloc(vec, element_size, vec->length, allocator, error);
}

//*********************************UNUSED*WRAPPER************************************************/
//
static void vec_internal_dummy_wrapper_wrapper__(void);
static void vec_internal_dummy_wrapper__(void) {
  vec_init(NULL, 0, 0, NULL, NULL);
  vec_deinit(NULL, 0, NULL);
  vec_more(NULL, 0, NULL, NULL);
  vec_push(NULL, 0, NULL, NULL, NULL);
  vec_pop(NULL, 0, NULL);
  vec_insert(NULL, 0, 0, NULL, NULL, NULL);
  vec_remove(NULL, 0, 0);
  vec_clear(NULL, 0);
  vec_reserve(NULL, 0, 0, NULL, NULL);
  vec_shrink(NULL, 0, NULL, NULL);
  vec_internal_dummy_wrapper_wrapper__();
}

static void vec_internal_dummy_wrapper_wrapper__(void) {
  vec_internal_dummy_wrapper__();
}

#endif // VEC_H_
