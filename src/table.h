#ifndef TABLE_H_
#define TABLE_H_

#include "allocator.h"
#include "builtin.h"
#include "debug_check.h"
#include "error.h"
#include "types.h"

#include <emmintrin.h>

/***
 * @doc(type): table_element_insert_f
 * @tag: all
 *
 * @brief: Callback for inserting elements into the table.
 *
 * @detailed: This callback is used by `table_insert` and `table_upsert` when we
 * are inserting an element into the table, which is not already present.
 * `table_element_insert_f` is required to prepare `dest` in such a way that the
 * `dest` element can later be identified via `table_element_compare_f`. You can
 * think of this function as the constructor of the elements in the table.
 *
 * @param(dest): destination element, pointer to the position where the `*src`
 * is to be inserted.
 * @assert(dest): `dest != NULL`
 * @assert(dest): `dest` will hold a valid memory address
 *
 * @param(src): source element, pointer to the element which is to be inserted
 * into `*dest`.
 * @assert(src): `src != NULL`
 * @assert(src): `src` will hold a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the vtable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*table_element_insert_f)(void *dest, void *src, void *ctx);

/**
 * @doc(type): table_element_overwrite_f
 * @tag: all
 *
 * @brief: Callback for overwriting elements in the table.
 *
 * @detailed: This callback is used by `table_insert` to overwrite elements in
 * the table which are already present. `table_element_overwrite_f` is required
 * to leave `dest` in such a state that it can later be identified by
 * `table_element_compare_f`
 *
 * @param(dest): destination pointer which will be overwritten by `src`
 * @assert(dest): `dest != NULL`
 * @assert(dest): `dest` will hold a valid memory address
 *
 * @param(src): source element, pointer to the element which will overwrite
 * `*dest`
 * @assert(src): `src != NULL`
 * @assert(src): `src` will hold a valid memory address
 *
 * @param(ctx): context pointer which will be inserted from the context pointer
 * inside the vtable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*table_element_overwrite_f)(void *dest, void *src, void *ctx);

/**
 * @doc(type): table_element_destroy_f
 * @tag: all
 *
 * @brief: Callback for destroying elements in the table.
 *
 * @detailed: Callback for destoying elements in the table. It will be called by
 * `table_remove` on the removed element and by `table_destroy` on each element.
 * It may be `NULL` in which case it will never get called.
 *
 * @param(element): element which is to be destroyed
 * @assert(element): `element != NULL`
 * @assert(element): element will always contain a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the vtable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*table_element_destroy_f)(void *element, void *ctx);

/**
 * @doc(type): table_element_compare_f
 * @tag: all
 *
 * @brief: Callback used for identifying elements in the table
 *
 * @param(first): points to the first element
 * @assert(first): `first != NULL`
 * @assert(first): first always holds a valid memory address
 *
 * @param(second): points to the second element
 * @assert(second): `second != NULL`
 * @assert(second): second always holds a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the vtable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef bool (*table_element_compare_f)(void *first, void *second, void *ctx);

/***
 * @doc(type): table_element_hash_f
 * @tag: all
 *
 * @brief: Callback used for hashing elements in the table.
 *
 * @param(element): element which is to be hashed
 * @assert(element): `element != NULL`
 * @assert(element): element will always point to a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the vtable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef u64 (*table_element_hash_f)(void *element, void *ctx);

/***
 * @doc(type): TableVTable
 * @tag: all
 *
 * @brief: Struct for generically handling elements in the table.
 *
 * @detailed: Struct which provides function pointers to the table which it then
 * uses internally to manage its elements. Each type is described seperately.
 *
 * @member(element_size): `sizeof(element)`. it should represent the object size
 * of each element in bytes.
 * @assert(element_size): `element_size > 0`
 *
 * @member(destroy): callback for destroying elements. `destoy` may be `NULl` in
 * which case no destructor will be called. This will make `table_deinit`
 * significantly faster
 *
 * @member(hash): callback for hashing elements
 * @assert(hash): `hash != NULL`
 *
 * @member(insert): callback for inserting elements into the table. Can be
 * thought of as the constructor of elements in the table
 * @assert(insert): `insert != NULL`
 *
 * @member(compare): callback for comparing elements inside the table
 * @assert(compare): `compare != NULL`
 *
 * @member(overwrite): callback for overwriting elements inside the table.
 * Useful for maps which need to update the value but not the key
 * @assert(overwrite): `overwrite != NULL`
 *
 * @member(ctx): context pointer passed to each callback. May be `NULL`.
 */
typedef struct TableVTable TableVTable;
struct TableVTable {
  table_element_hash_f hash;
  table_element_insert_f insert;
  table_element_compare_f compare;
  table_element_destroy_f destroy;
  table_element_overwrite_f overwrite;
  usize element_size;
  void *ctx;
};

/**
 * @doc(type): Table
 * @tag: all
 *
 * @brief Opaque pointer to a table struct.
 *
 * @detailed: Opaque pointer to a table struct.
 * @assert: table struct has to follow the form `Table(TYPE)`
 */
typedef void Table;

/***
 * @doc(type): Table(TYPE)
 * @tag: all
 *
 * @brief struct type for a table with elements of type `TYPE`
 *
 * @param(TYPE)
 */
#define Table(TYPE)                                                            \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
    usize tombs;                                                               \
  }

// TODO: documentation
#define TABLE_INTERNAL_CONTROL_ISSET_MASK ((u8)(1 << 7))
#define TABLE_INTERNAL_CONTROL_TOMB ((u8)1)
#define TABLE_INTERNAL_CONTROL_FREE ((u8)0)

// TODO: documentation
static u8 table_internal_hash_to_control_byte(u64 hash) {
  return (hash & 255) | TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

// TODO: documentation
static usize table_internal_end_from_capacity(usize capacity) {
  debug_check(capacity > 0);
  capacity *= 8;
  capacity /= 7;
  return 1 << (8 * sizeof(unsigned long long) -
               builtin_clzll((unsigned long long)capacity));
}

// TODO: documentation
// TODO: assert
static byte *table_internal_control_array(Table *table_, TableVTable *vtable) {
  debug_check(table_);
  debug_check(vtable);

  Table(byte) *table = table_;
  return table->element + vtable->element_size * table->end;
}

// TODO: documentation
// TODO: assert
static void table_deinit(Table *table_, Allocator *allocator) {
  debug_check(table_);
  debug_check(allocator);

  Table(byte) *table = table_;
  allocator_free(allocator, table->element);
}

// TODO: documentation
// TODO: assert
static void table_internal_init(Table *table_, TableVTable *vtable, usize end,
                                Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(end > 0); // assert end is power of two
  debug_check(allocator);

  Table(byte) *table = table_;
  builtin_memset(table, 0, sizeof(*table));
  table->end = end;

  usize chunk_size = vtable->element_size * table->end;
  chunk_size += table->end + 16;

  table->element = allocator_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  byte *control = table_internal_control_array(table, vtable);
  builtin_memset(control, TABLE_INTERNAL_CONTROL_FREE, 16 + table->end);
}

static void table_init(Table *table, TableVTable *vtable,
                       usize initial_capacity, Allocator *allocator,
                       Error *error) {
  // TODO: this way if end wil always be over 16 this should not be the case if
  // initial capacity can be satisfied with end == 16
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }
  usize end = table_internal_end_from_capacity(initial_capacity);
  table_internal_init(table, vtable, end, allocator, error);
}

static usize table_internal_find(Table *table_, TableVTable *vtable,
                                 void *element, u64 hash) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(element);

  Table(byte) *table = table_;
  const usize mask = table->end - 1;
  usize index = hash & mask;
  const __m128i control_mask =
      _mm_set1_epi8(table_internal_hash_to_control_byte(hash));
  const __m128i zero = _mm_set1_epi8(0);
  const byte *control = table_internal_control_array(table, vtable);

  while (1) {
    const __m128i control_data =
        _mm_loadu_si128((const __m128i_u *)(control + index));
    usize poss_bitmask =
        _mm_movemask_epi8(_mm_cmpeq_epi8(control_mask, control_data));

    if (LIKELY(poss_bitmask)) {
      usize group_index = 0;
      do {
        usize ctz = builtin_ctz(poss_bitmask);
        group_index += ctz;
        poss_bitmask >>= ctz;

        usize real_index = (index + group_index) & mask;

        if (vtable->compare(element,
                            &table->element[vtable->element_size * real_index],
                            vtable->ctx)) {
          return real_index;
        }
        group_index += 1;
        poss_bitmask >>= 1;
      } while (poss_bitmask);
    }

    usize empty_bitmask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, control_data));
    if (LIKELY(empty_bitmask)) {
      return (index + builtin_ctz(empty_bitmask)) & mask;
    }
    index += 16;
    index &= mask;
  }
}

static usize table_find(Table *table, TableVTable *vtable, void *element) {
  debug_check(table);
  debug_check(vtable);
  debug_check(element);

  return table_internal_find(table, vtable, element,
                             vtable->hash(element, vtable->ctx));
}

static bool table_isset(Table *table, TableVTable *vtable, usize index) {
  debug_check(table);
  debug_check(vtable);
  debug_check(((Table(byte) *)table)->end > index);

  return table_internal_control_array(table, vtable)[index] &
         TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

// TODO: table_remove

static bool table_contains(Table *table_, TableVTable *vtable, void *element) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(element);

  usize index = table_find(table_, vtable, element);
  return table_isset(table_, vtable, index);
}

static void table_internal_realloc(Table *table_, TableVTable *vtable,
                                   usize end, Allocator *allocator,
                                   Error *error) {

  debug_check(table_);
  debug_check(vtable);
  debug_check(end > 0); // TODO: check if end is power of two
  debug_check(allocator);

  // TODO: does this increase capacity more than it should?
  Table(byte) *table = table_;

  Table(byte) table_new;

  table_internal_init(&table_new, vtable, end, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  table_new.length = table->length;

  byte *table_new_control = table_internal_control_array(&table_new, vtable);
  byte *table_control = table_internal_control_array(table, vtable);

  for (usize i = 0; i < table->end; ++i) {
    if (UNLIKELY(!(table_control[i] & TABLE_INTERNAL_CONTROL_ISSET_MASK))) {
      continue;
    }

    byte *element = table->element + vtable->element_size * i;
    const u64 hash = vtable->hash(element, vtable->ctx);
    const usize j = table_internal_find(&table_new, vtable, element, hash);
    builtin_memcpy(table_new.element + vtable->element_size * j, element,
                   vtable->element_size);
    table_new_control[j] = table_internal_hash_to_control_byte(hash);
    if (UNLIKELY(j < 16)) {
      table_new_control[j + table_new.end] =
          table_internal_hash_to_control_byte(hash);
    }
  }

  allocator_free(allocator, table->element);
  builtin_memcpy(table, &table_new, sizeof(*table));
}

static void table_shrink(Table *table_, TableVTable *vtable,
                         Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(allocator);

  Table(byte) *table = table_;
  usize end = table_internal_end_from_capacity(table->length);
  table_internal_realloc(table, vtable, end, allocator, error);
}

static void table_reserve(Table *table_, TableVTable *vtable, usize capacity,
                          Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(capacity > 0);
  debug_check(allocator);

  Table(byte) *table = table_;
  usize end = table_internal_end_from_capacity(capacity);
  if (end <= table->end) {
    return;
  }

  table_internal_realloc(table, vtable, end, allocator, error);
}

static void table_internal_should_grow(Table *table_, TableVTable *vtable,
                                       Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(allocator);

  Table(byte) *table = table_;
  if (table->length + table->tombs >= table->end - table->end / 8) {
    table_internal_realloc(table, vtable, table->end * 2, allocator, error);
  }
}

static void table_internal_insert(Table *table_, TableVTable *vtable,
                                  void *element, u64 hash, usize index) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(element);
  debug_check(((Table(byte) *)table_)->end > index);

  Table(byte) *table = table_;
  byte *control = table_internal_control_array(table, vtable);
  control[index] = table_internal_hash_to_control_byte(hash);
  if (index < 16) {
    control[index + table->end] = table_internal_hash_to_control_byte(hash);
  }
  vtable->insert(table->element + index * vtable->element_size, element,
                 vtable->ctx);
  table->length += 1;
}

static usize table_insert(Table *table_, TableVTable *vtable, void *element,
                          Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(element);
  debug_check(allocator);

  Table(byte) *table = table_;
  table_internal_should_grow(table, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u64 hash = vtable->hash(element, vtable->ctx);
  usize index = table_internal_find(table, vtable, element, hash);

  byte *control = table_internal_control_array(table, vtable);

  if (control[index] & TABLE_INTERNAL_CONTROL_ISSET_MASK) {
    vtable->overwrite(table->element + index * vtable->element_size, element,
                      vtable->ctx);
  } else {
    table_internal_insert(table, vtable, element, hash, index);
  }
  return index;
}

static usize table_upsert(Table *table_, TableVTable *vtable, void *element,
                          Allocator *allocator, Error *error) {
  debug_check(table_);
  debug_check(vtable);
  debug_check(element);
  debug_check(allocator);

  Table(byte) *table = table_;
  table_internal_should_grow(table, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u64 hash = vtable->hash(element, vtable->ctx);
  byte *control = table_internal_control_array(table, vtable);
  usize index = table_internal_find(table, vtable, element, hash);
  if (!(control[index] & TABLE_INTERNAL_CONTROL_ISSET_MASK)) {
    table_internal_insert(table, vtable, element, hash, index);
  }
  return index;
}

#endif // TABLE_H_
