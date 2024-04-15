#ifndef VEC_H_
#define VEC_H_

#include "../allocator/allocator.h"
#include "../error/error.h"
#include "../holy_types/holy_types.h"

#define vec_create(ALLOC, TYPE, INIT_CAP, ERR)                                 \
  (TYPE *)vec_create_impl(ALLOC, sizeof(TYPE), INIT_CAP, ERR)

__attribute__((malloc)) void *vec_create_impl(Allocator *allocator,
                                              usize object_size,
                                              usize initial_capacity,
                                              Error **error);

/* #define vec_more(VEC, ALLOC, ERROR) \ */
/*   (((typeof(**(VEC)))*)vec_more_impl((void **)(VEC), ALLOC, sizeof(**(VEC)),
 * \ */
/*                                      ERR)) */
#define vec_more(VEC, ALLOC, ERROR)                                            \
  (typeof(**(VEC)) *)vec_more_impl((void **)(VEC), ALLOC, sizeof(**(VEC)),     \
                                   ERROR)
void *vec_more_impl(void **vec, Allocator *allocator, usize object_size,
                    Error **error);

#define vec_length(VEC) vec_length_impl((void **)VEC)
usize vec_length_impl(void **vec);

#define vec_reserve(VEC, ALLOC, NUM, ERR)                                      \
  vec_reserve_impl((void **)VEC, ALLOC, sizeof(**(VEC)), NUM, ERR)
void vec_reserve_impl(void **vec, Allocator *allocator, usize object_size,
                      usize to_reserve, Error **error);

/* #define vec_insert(VEC, ALLOC, INDEX, ERROR) \ */
/*   (((typeof(**(VEC)))*)vec_insert_impl((void **)(VEC), ALLOC, INDEX, \ */
/*                                        sizeof(**(VEC)), ERR)) */
#define vec_insert(VEC, ALLOC, INDEX, ERROR)                                   \
  (typeof(**(VEC)) *)vec_insert_impl((void **)VEC, ALLOC, INDEX,               \
                                     sizeof(**(VEC)), ERROR)
void *vec_insert_impl(void **vec, Allocator *allocator, usize index,
                      usize object_size, Error **error);

#define vec_shrink(VEC, ALLOC, ERR)                                            \
  vec_shrink_impl((void **)(VEC), ALLOC, sizeof(**(VEC)), ERR)
void vec_shrink_impl(void **vec, Allocator *allocator, usize object_size,
                     Error **error);

#define vec_destroy(VEC, ALLOC, ERR)                                           \
  vec_destroy_impl((void **)(VEC), ALLOC, ERR);
void vec_destroy_impl(void **vec, Allocator *allocator, Error **error);

#define vec_remove(VEC, INDEX)                                                 \
  vec_remove_impl((void **)(VEC), INDEX, sizeof(**(VEC)))
void vec_remove_impl(void **vec, usize index, usize object_size);

#endif // VEC_H_
