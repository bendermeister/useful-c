#ifndef UCX_H_
#define UCX_H_

#include <uc/types.h>

// ********************************Error****************************************
typedef int ucx_Error;

// ********************************Allocator************************************
typedef void ucx_Allocator;

extern ucx_Allocator *ucx_allocator_global;

void *ucx_allocator_alloc(ucx_Allocator *allocator, usize chunk_size,
                          ucx_Error *error);
void *ucx_allocator_realloc(ucx_Allocator *allocator, void *chunk,
                            usize chunk_size, ucx_Error *error);
void ucx_allocator_free(ucx_Allocator *allocator, void *chunk);
#endif // UCX_H_

// ********************************Vec******************************************
typedef void ucx_Vec;

#define ucx_Vec(TYPE)                                                          \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
  }
void ucx_vec_init(ucx_Vec *vec, usize element_size, usize initial_capacity,
                  ucx_Allocator *allocator, ucx_Error *error);

void ucx_vec_deinit(ucx_Vec *vec, usize element_size, ucx_Allocator *allocator);
void *ucx_vec_more(ucx_Vec *vec, usize element_size, ucx_Allocator *allocator,
                   ucx_Error *error);
void ucx_vec_push(ucx_Vec *vec, usize element_size, const void *element,
                  ucx_Allocator *allocator, ucx_Error *error);
void ucx_vec_pop(ucx_Vec *vec, usize element_size, void *dest);
void ucx_vec_insert(ucx_Vec *vec, usize element_size, usize index,
                    const void *element, ucx_Allocator *allocator,
                    ucx_Error *error);
void ucx_vec_remove(ucx_Vec *vec, usize element_size, usize index);
void ucx_vec_clear(ucx_Vec *vec, usize element_size);
void ucx_vec_reserve(ucx_Vec *vec, usize element_size, usize new_capacity,
                     ucx_Allocator *allocator, ucx_Error *error);
void ucx_vec_shrink(ucx_Vec *vec, usize element_size, ucx_Allocator *allocator,
                    ucx_Error *error);

// ********************************Table*****************************************

typedef void (*ucx_table_element_insert_f)(void *dest, const void *src,
                                           void *ctx);
typedef void (*ucx_table_element_overwrite_f)(void *dest, const void *src,
                                              void *ctx);
typedef void (*ucx_table_element_destroy_f)(void *element, void *ctx);
typedef bool (*ucx_table_element_compare_f)(const void *first,
                                            const void *second, void *ctx);
typedef u64 (*ucx_table_element_hash_f)(const void *element, void *ctx);
typedef struct ucx_TableVTable ucx_TableVTable;
struct ucx_TableVTable {
  ucx_table_element_hash_f hash;
  ucx_table_element_insert_f insert;
  ucx_table_element_compare_f compare;
  ucx_table_element_destroy_f destroy;
  ucx_table_element_overwrite_f overwrite;
  usize element_size;
  void *ctx;
};
typedef void ucx_Table;
#define ucx_Table(TYPE)                                                        \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
    usize tombs;                                                               \
  }

void ucx_table_deinit(ucx_Table *table_, ucx_Allocator *allocator);

void ucx_table_init(ucx_Table *table, const ucx_TableVTable *vtable,
                    usize initial_capacity, ucx_Allocator *allocator,
                    ucx_Error *error);

usize ucx_table_find(const ucx_Table *table, const ucx_TableVTable *vtable,
                     const void *element);

bool ucx_table_isset(const ucx_Table *table, const ucx_TableVTable *vtable,
                     usize index);

bool ucx_table_contains(const ucx_Table *table_, const ucx_TableVTable *vtable,
                        const void *element);

void ucx_table_shrink(ucx_Table *table_, const ucx_TableVTable *vtable,
                      ucx_Allocator *allocator, ucx_Error *error);

void ucx_table_reserve(ucx_Table *table_, const ucx_TableVTable *vtable,
                       usize capacity, ucx_Allocator *allocator,
                       ucx_Error *error);

usize ucx_table_insert(ucx_Table *table_, const ucx_TableVTable *vtable,
                       const void *element, ucx_Allocator *allocator,
                       ucx_Error *error);
usize ucx_table_upsert(ucx_Table *table_, const ucx_TableVTable *vtable,
                       const void *element, ucx_Allocator *allocator,
                       ucx_Error *error);
