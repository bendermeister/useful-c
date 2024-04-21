#include <uc/allocator.h>
#include <uc/table.h>
/***
 * @file
 * Wrapper to compile UC libs in a single translation unit and later link it
 * with the other object files. Also adds the prefix `ucx_` to every function,
 * macro and type.
 *
 */
#include <uc/types.h>
#include <uc/vec.h>

#include <uc/ucx.h>

// ********************************Allocator*************************************
/***
 * @brief Allocate a chunk of memory through `allocator`
 *
 * Allocates a chunk of memory through the provided allocator `allocator`. It is
 * at least `chunk_size` big.
 *
 * @param allocator Allocator which is used to allocate the chunk
 * Assert:
 * - `allocator != NULL`
 * - `allocator` must be a valid allocator
 *
 * @return returns pointer to the allocated chunk of memory, or undefined in
 * case of an error
 */
void *ucx_allocator_alloc(ucx_Allocator *allocator, usize chunk_size,
                          ucx_Error *error) {
  return allocator_alloc(allocator, chunk_size, error);
}

void *ucx_allocator_realloc(ucx_Allocator *allocator, void *chunk,
                            usize chunk_size, ucx_Error *error) {
  return allocator_realloc(allocator, chunk, chunk_size, error);
}

void ucx_allocator_free(ucx_Allocator *allocator, void *chunk) {
  allocator_free(allocator, chunk);
}

ucx_Allocator *ucx_allocator_global = &(AllocatorInternal){
    .vtable =
        &(AllocatorVTable){
            .free = allocator_internal_global_free,
            .alloc = allocator_internal_global_alloc,
            .realloc = allocator_internal_global_realloc,
        },
};

// ********************************Vec******************************************
void ucx_vec_init(ucx_Vec *vec, usize element_size, usize initial_capacity,
                  ucx_Allocator *allocator, ucx_Error *error) {
  vec_init(vec, element_size, initial_capacity, allocator, error);
}

void ucx_vec_deinit(ucx_Vec *vec, usize element_size,
                    ucx_Allocator *allocator) {
  vec_deinit(vec, element_size, allocator);
}

void *ucx_vec_more(ucx_Vec *vec, usize element_size, ucx_Allocator *allocator,
                   ucx_Error *error) {
  return vec_more(vec, element_size, allocator, error);
}

void ucx_vec_push(ucx_Vec *vec, usize element_size, const void *element,
                  ucx_Allocator *allocator, ucx_Error *error) {
  vec_push(vec, element_size, element, allocator, error);
}

void ucx_vec_pop(ucx_Vec *vec, usize element_size, void *dest) {
  vec_pop(vec, element_size, dest);
}

void ucx_vec_insert(ucx_Vec *vec, usize element_size, usize index,
                    const void *element, ucx_Allocator *allocator,
                    ucx_Error *error) {
  vec_insert(vec, element_size, index, element, allocator, error);
}

void ucx_vec_remove(ucx_Vec *vec, usize element_size, usize index) {
  vec_remove(vec, element_size, index);
}

void ucx_vec_clear(ucx_Vec *vec, usize element_size) {
  vec_clear(vec, element_size);
}

void ucx_vec_reserve(ucx_Vec *vec, usize element_size, usize new_capacity,
                     ucx_Allocator *allocator, ucx_Error *error) {
  vec_reserve(vec, element_size, new_capacity, allocator, error);
}

void ucx_vec_shrink(ucx_Vec *vec, usize element_size, ucx_Allocator *allocator,
                    ucx_Error *error) {
  vec_shrink(vec, element_size, allocator, error);
}

// ********************************Table****************************************

void ucx_table_deinit(ucx_Table *table_, ucx_Allocator *allocator) {
  table_deinit(table_, allocator);
}

void ucx_table_init(ucx_Table *table, const ucx_TableVTable *vtable,
                    usize initial_capacity, ucx_Allocator *allocator,
                    ucx_Error *error) {
  table_init(table, (void *)vtable, initial_capacity, allocator, error);
}

usize ucx_table_find(const ucx_Table *table, const ucx_TableVTable *vtable,
                     const void *element) {
  return table_find(table, (void *)vtable, element);
}

bool ucx_table_isset(const ucx_Table *table, const ucx_TableVTable *vtable,
                     usize index) {
  return table_isset(table, (void *)vtable, index);
}

bool ucx_table_contains(const ucx_Table *table_, const ucx_TableVTable *vtable,
                        const void *element) {
  return table_contains(table_, (void *)vtable, element);
}

void ucx_table_shrink(ucx_Table *table_, const ucx_TableVTable *vtable,
                      ucx_Allocator *allocator, ucx_Error *error) {
  table_shrink(table_, (void *)vtable, allocator, error);
}

void ucx_table_reserve(ucx_Table *table_, const ucx_TableVTable *vtable,
                       usize capacity, ucx_Allocator *allocator,
                       ucx_Error *error) {
  table_reserve(table_, (void *)vtable, capacity, allocator, error);
}

usize ucx_table_insert(ucx_Table *table_, const ucx_TableVTable *vtable,
                       const void *element, ucx_Allocator *allocator,
                       ucx_Error *error) {
  return table_insert(table_, (void *)vtable, element, allocator, error);
}

usize ucx_table_upsert(ucx_Table *table_, const ucx_TableVTable *vtable,
                       const void *element, ucx_Allocator *allocator,
                       ucx_Error *error) {
  return table_upsert(table_, (void *)vtable, element, allocator, error);
}
