#ifndef TABLE_H_
#define TABLE_H_

#include "../allocator/allocator.h"
#include "../holy_types/holy_types.h"
#include "../macro_util/macro_util.h"

// TODO: what if user doesn't have this fancy tech?
#include <emmintrin.h>

#include <stdbool.h>
#include <stddef.h>

typedef void Table;

// TODO: destroy

/***
 * \typedef
 * \brief Callback to compare entries in the table
 *
 * Callback to compare entries in the table.
 * \param a pointer to first entry. `a` will never be `NULL`.
 * \param b pointer to second entry. `b` will never be `NULL`.
 * \return `true` if entry `a` equals entry `b`, `false` if not.
 */
typedef bool (*Table_Compare_Func)(void *a, void *b);

/***
 * \typedef
 * \brief Callback to hash entries in the table
 *
 * Callback to hash entries in the table.
 * \param entry pointer to entry that needs to be hashed. `entry` will never be
 * `NULL`.
 * \return hash value associated with `entry`
 */
typedef u32 (*Table_Hash_Func)(void *entry);

/***
 * \typedef
 * \brief Callback to insert entries into the table
 *
 * Callback to insert entries into the table.
 * \param dest destination pointer to an entry inside the table. `dest` will
 * never be `NULL`
 * \param src source pointer to an entry outside the table. `src` will never be
 * NULL
 */
typedef void (*Table_Insert_Func)(void *dest, void *src);

/***
 * \typedef
 * \brief Callback to overwrite entries into the table
 *
 * Callback to overwrite entries into the table. It is called for example on
 * `table_insert` when there is already an entry in the table which satisfies:
 * `compare(a, b) == 1` and is useful for example for map inserts where
 * overwrite should overwrite the value part of an entry but not the key part.
 *
 * \param dest destination pointer to an entry inside the table. `dest` will
 * never be `NULL`
 * \param src source pointer to an entry outside the table. `src` will never be
 * NULL
 */
typedef void (*Table_Overwrite_Func)(void *dest, void *src);

/***
 * \type
 * \brief virtual table of a table
 *
 * Table_VTable describes the several functions and constants which the table
 * calls internally. The vtable for a given `Table_Type(xxx)` can be reused for
 * other tables of the same type. But should never be used for tables of
 * different types.
 *
 */
typedef struct Table_VTable Table_VTable;
struct Table_VTable {
  Table_Compare_Func compare;
  Table_Hash_Func hash;
  Table_Insert_Func insert;
  Table_Overwrite_Func overwrite;
  u32 entry_size;
};

/***
 * This generates an anonymous struct with your type information in it. This way
 * you can access elements in the table by index to `table.entry[i]`
 *
 * \param TYPE The entry type of this table.
 */
#define Table_Type(TYPE)                                                       \
  struct {                                                                     \
    u32 length;                                                                \
    u32 tombs;                                                                 \
    u32 end;                                                                   \
    TYPE *entry;                                                               \
  }

#define TABLE_INTERNAL_CONTROL_ISSET_MASK ((u8)(1 << 7))
#define TABLE_INTERNAL_CONTROL_TOMB ((u8)1)
#define TABLE_INTERNAL_CONTROL_FREE ((u8)0)

[[gnu::const]] [[nodiscard]] [[gnu::nothrow]] static u8
table_internal_hash_to_control(u32 hash) {
  return (hash & 255) | TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

[[gnu::const]] [[nodiscard]] [[gnu::nothrow]] static u32
table_internal_end_from_capacity(u32 capacity) {
  capacity *= 4;
  capacity /= 3;
  return 1 << (8 * sizeof(unsigned long long) -
               __builtin_clzll((unsigned long long)capacity));
}

[[gnu::nothrow]] [[nodiscard]] static byte *
table_internal_control(Table *table_, Table_VTable *vtable) {

  Table_Type(byte) *table = table_;
  return table->entry + vtable->entry_size * table->end;
}

[[maybe_unused]] [[gnu::nothrow]] static void
table_deinit(Table *table_, Allocator *allocator, Error **error) {
  Table_Type(byte) *table = table_;
  allocator_free(allocator, table->entry, error);
}

[[maybe_unused]] [[gnu::nothrow]] static Table *
table_init(Table *table_, Table_VTable *vtable, u32 initial_capacity,
           Allocator *allocator, Error **error) {
  // TODO: this way if end wil always be over 16 this should not be the case if
  // initial capacity can be satisfied with end == 16
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }

  Table_Type(byte) *table = table_;
  __builtin_memset(table, 0, sizeof(*table));
  table->end = table_internal_end_from_capacity(initial_capacity);

  u32 chunk_size = vtable->entry_size * table->end;
  chunk_size += table->end + 16;

  table->entry = allocator_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  byte *control = table_internal_control(table, vtable);
  __builtin_memset(control, TABLE_INTERNAL_CONTROL_FREE, 16 + table->end);
  return table;
}

// TODO: comment that explains this
[[gnu::hot]] [[gnu::nothrow]] [[nodiscard]] static u32
table_internal_find(Table *table_, Table_VTable *vtable, void *entry,
                    u32 hash) {
  Table_Type(byte) *table = table_;
  const u32 mask = table->end - 1;
  u32 index = hash & mask;
  const __m128i control_mask =
      _mm_set1_epi8(table_internal_hash_to_control(hash));
  const __m128i zero = _mm_set1_epi8(0);
  const byte *control = table_internal_control(table, vtable);

  while (1) {
    const __m128i control_data =
        _mm_loadu_si128((const __m128i_u *)(control + index));
    u32 poss_bitmask =
        _mm_movemask_epi8(_mm_cmpeq_epi8(control_mask, control_data));

    if (LIKELY(poss_bitmask)) {
      u32 group_index = 0;
      do {
        u32 ctz = __builtin_ctz(poss_bitmask);
        group_index += ctz;
        poss_bitmask >>= ctz;

        u32 real_index = (index + group_index) & mask;

        if (vtable->compare(entry,
                            &table->entry[vtable->entry_size * real_index])) {
          return real_index;
        }
        group_index += 1;
        poss_bitmask >>= 1;
      } while (poss_bitmask);
    }

    u32 empty_bitmask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, control_data));
    if (LIKELY(empty_bitmask)) {
      return (index + __builtin_ctz(empty_bitmask)) & mask;
    }
    index += 16;
    index &= mask;
  }
}

[[maybe_unused]] [[gnu::nothrow]] [[nodiscard]] static u32
table_find(Table *table, Table_VTable *vtable, void *entry) {
  return table_internal_find(table, vtable, entry, vtable->hash(entry));
}

[[maybe_unused]] [[gnu::nothrow]] [[nodiscard]] static bool
table_isset(Table *table, Table_VTable *vtable, usize index) {
  return table_internal_control(table, vtable)[index] &
         TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

// TODO: table_remove

[[maybe_unused]] [[gnu::nothrow]] [[nodiscard]] static bool
table_contains(Table *table_, Table_VTable *vtable, void *entry) {
  usize index = table_find(table_, vtable, entry);
  return table_isset(table_, vtable, index);
}

[[maybe_unused]] [[gnu::nothrow]] [[nodiscard]] static u32
table_inc_index(Table *table_, Table_VTable *vtable, u32 i) {
  i += 1;
  Table_Type(byte) *table = table_;
  const byte *control = table_internal_control(table_, vtable);
  for (; i < table->end; ++i) {
    if (LIKELY(control[i] & TABLE_INTERNAL_CONTROL_ISSET_MASK)) {
      return i;
    }
  }
  return table->end;
}

[[maybe_unused]] [[gnu::nothrow]] [[nodiscard]] static u32
table_first_index(Table *table_, Table_VTable *vtable) {
  return table_inc_index(table_, vtable, -1);
}

[[gnu::nothrow]] static void
table_internal_realloc(Table *table_, Table_VTable *vtable, u32 capacity,
                       Allocator *allocator, Error **error) {

  Table_Type(byte) *table = table_;

  Table_Type(byte) table_new;

  table_init(&table_new, vtable, capacity, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  // TODO: simd for loop optimization is necessary?

  byte *table_new_control = table_internal_control(&table_new, vtable);

  for (u32 i = table_first_index(table, vtable); i < table->end;
       i = table_inc_index(table, vtable, i)) {

    byte *entry = table->entry + vtable->entry_size * i;
    const u32 hash = vtable->hash(entry);
    const u32 j = table_internal_find(&table_new, vtable, entry, hash);
    __builtin_memcpy(table_new.entry + vtable->entry_size * j, entry,
                     vtable->entry_size);
    table_new_control[j] = table_internal_hash_to_control(hash);
    if (j < 16) {
      table_new_control[j + table->end] = table_internal_hash_to_control(hash);
    }
  }

  // TODO should allocator free should not be able to error this is just
  // retarded

  allocator_free(allocator, table->entry, error);
  __builtin_memcpy(table, &table_new, sizeof(*table));
}

[[maybe_unused]] [[gnu::nothrow]] static void table_shrink(Table *table_,
                                                           Table_VTable *vtable,
                                                           Allocator *allocator,
                                                           Error **error) {
  Table_Type(byte) *table = table_;
  table_internal_realloc(table, vtable, table->length, allocator, error);
}

[[maybe_unused]] [[gnu::nothrow]] static void
table_reserve(Table *table_, Table_VTable *vtable, u32 capacity,
              Allocator *allocator, Error **error) {
  Table_Type(byte) *table = table_;
  u32 end = table_internal_end_from_capacity(capacity);
  if (end <= table->end) {
    return;
  }

  table_internal_realloc(table, vtable, capacity, allocator, error);
}

[[gnu::nothrow]] static void table_internal_should_grow(Table *table_,
                                                        Table_VTable *vtable,
                                                        Allocator *allocator,
                                                        Error **error) {
  Table_Type(byte) *table = table_;
  if (table->length + table->tombs >= table->end - table->end / 4) {
    table_internal_realloc(table, vtable, table->end * 2, allocator, error);
  }
}

[[maybe_unused]] [[gnu::nothrow]] static void
table_internal_insert(Table *table_, Table_VTable *vtable, void *entry,
                      u32 hash, u32 index) {
  Table_Type(byte) *table = table_;
  byte *control = table_internal_control(table, vtable);
  control[index] = table_internal_hash_to_control(hash);
  if (index < 16) {
    control[index + table->end] = table_internal_hash_to_control(hash);
  }
  vtable->insert(table->entry + index * vtable->entry_size, entry);
  table->length += 1;
}

[[maybe_unused]] [[gnu::nothrow]] static u32
table_insert(Table *table_, Table_VTable *vtable, void *entry,
             Allocator *allocator, Error **error) {
  Table_Type(byte) *table = table_;

  table_internal_should_grow(table, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u32 hash = vtable->hash(entry);
  u32 index = table_internal_find(table, vtable, entry, hash);

  byte *control = table_internal_control(table, vtable);

  if (control[index] & TABLE_INTERNAL_CONTROL_ISSET_MASK) {
    vtable->overwrite(table->entry + index * vtable->entry_size, entry);
  } else {
    table_internal_insert(table, vtable, entry, hash, index);
  }
  return index;
}

[[maybe_unused]] [[gnu::nothrow]] static u32
table_upsert(Table *table_, Table_VTable *vtable, void *entry,
             Allocator *allocator, Error **error) {
  Table_Type(byte) *table = table_;
  table_internal_should_grow(table, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u32 hash = vtable->hash(entry);
  byte *control = table_internal_control(table, vtable);
  u32 index = table_internal_find(table, vtable, entry, hash);
  if (!(control[index] & TABLE_INTERNAL_CONTROL_ISSET_MASK)) {
    table_internal_insert(table, vtable, entry, hash, index);
  }
  return index;
}

// TODO: table_find

#endif // TABLE_H_
