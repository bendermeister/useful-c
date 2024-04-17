#ifndef TABLE_H_
#define TABLE_H_

#include "../allocator/allocator.h"
#include "../holy_types/holy_types.h"
#include "../macro_util/macro_util.h"

// TODO: what if user doesn't have this fancy tech?
#include <emmintrin.h>

#include <stddef.h>

typedef void Table;

typedef struct Table_VTable Table_VTable;
struct Table_VTable {
  bool (*compare)(void *, void *);
  u32 (*hash)(void *);
  void (*insert)(void *, void *);
  void (*overwrite)(void *, void *);
  u32 entry_size;
};

#define Table_Type(TYPE)                                                       \
  struct {                                                                     \
    u32 length;                                                                \
    u32 tombs;                                                                 \
    u32 end;                                                                   \
    TYPE *entry;                                                               \
  }

static constexpr u8 TABLE_INTERNAL_CONTROL_ISSET_MASK = 1 << 7;
static constexpr u8 TABLE_INTERNAL_CONTROL_TOMB = 1;
static constexpr u8 TABLE_INTERNAL_CONTROL_FREE = 0;

[[gnu::const]] [[nodiscard]] [[gnu::nothrow]] static u8
table_internal_hash_to_control(u32 hash) {
  return hash & 255 | TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

[[gnu::const]] [[nodiscard]] [[gnu::nothrow]] static u32
table_internal_end_from_capacity(u32 capacity) {
  capacity *= 4;
  capacity /= 3;
  return 1 << (sizeof(unsigned long long) - __builtin_clzll(capacity));
}

[[gnu::nothrow]] [[nodiscard]] static byte *
table_internal_control(Table *table_, Table_VTable *vtable) {

  Table_Type(byte) *table = table_;
  return table->entry + vtable->entry_size * table->end;
}

[[gnu::nothrow]] static void table_deinit(Table *table_, Allocator *allocator,
                                          Error **error) {
  Table_Type(byte) *table = table_;
  allocator_free(allocator, table->entry, error);
}

[[gnu::nothrow]] static Table *table_init(Table *table_, Table_VTable *vtable,
                                          u32 initial_capacity,
                                          Allocator *allocator, Error **error) {
  // TODO: this way if end wil always be over 16 this should not be the case if
  // initial capacity can be satisfied with end == 16
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }
  u32 end = table_internal_end_from_capacity(initial_capacity);

  Table_Type(byte) *table = table_;

  u32 chunk_size = vtable->entry_size * end;
  chunk_size += end + 16;

  table->entry = allocator_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return NULL;
  }

  byte *control = table_internal_control(table, vtable);
  __builtin_memset(control, TABLE_INTERNAL_CONTROL_FREE, 16 + end);
  table->end = end;
  table->tombs = 0;
  table->length = 0;
  return table;
}

// TODO: comment that explains this
[[gnu::hot]] [[gnu::nothrow]] [[nodiscard]] static u32
table_internal_find(Table *table_, Table_VTable *vtable, void *entry,
                    u32 hash) {
  Table_Type(byte) *table = table_;
  const u32 mask = table->end - 1;
  u32 index = hash & mask;
  const auto control_mask = _mm_set1_epi8(table_internal_hash_to_control(hash));
  const auto zero = _mm_set1_epi8(0);
  const byte *control = table_internal_control(table, vtable);

  while (1) {
    auto control_data = _mm_loadu_si128((const __m128i_u *)(control + index));
    auto poss_bitmask =
        _mm_movemask_pi8(_mm_cmpeq_epi8(control_mask, control_data));

    if (LIKELY(poss_bitmask)) {
      u32 group_index = 0;
      do {
        auto ctz = __builtin_ctx(poss_bitmask);
        group_index += ctz;
        poss_bitmask >>= ctz;

        u32 real_index = (index + group_index) & mask;

        if (vtable->compare(entry,
                            &table->entry[vtable->entry_size * real_index])) {
          return real_index;
        }

      } while (poss_bitmask);
    }

    auto empty_bitmask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, control_data));
    if (LIKELY(empty_bitmask)) {
      return (index + __builtin_ctz(empty_bitmask)) & mask;
    }
  }
}

[[gnu::nothrow]] [[nodiscard]] static u32
table_find(Table *table, Table_VTable *vtable, void *entry) {
  return table_internal_find(table, vtable, entry, vtable->hash(entry));
}

[[gnu::nothrow]] [[nodiscard]] static bool
table_isset(Table *table, Table_VTable *vtable, void *entry) {
  const u32 index = table_find(table, vtable, entry);
  return table_internal_control(table, vtable)[index] &
         TABLE_INTERNAL_CONTROL_ISSET_MASK;
}

[[gnu::nothrow]] [[nodiscard]] static bool
table_contains(Table *table_, Table_VTable vtable, void *entry) {
  Table_Type(byte) *table = table_;
}

[[gnu::nothrow]] [[nodiscard]] static u32
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

[[gnu::nothrow]] [[nodiscard]] static u32
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

  byte *table_control = table_internal_control(table, vtable);
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
  table->tomb = 0;
  table->entry = table_new->entry;
}

[[gnu::nothrow]] static void table_shrink(Table *table_, Table_VTable *vtable,
                                          Allocator *allocator, Error **error) {
  Table_Type(byte) *table = table_;
  table_internal_realloc(table, vtable, table->length, allocator, error);
}

[[gnu::nothrow]] static void table_reserve(Table *table_, Table_VTable *vtable,
                                           u32 capacity, Allocator *allocator,
                                           Error **error) {
  Table_Type(byte) *table = table_;
  u32 end = table_internal_end_from_capacity(capacity);
  if (end <= table->end) {
    return;
  }

  table_internal_realloc(table, vtable, capacity, allocator, error);
}

[[gnu::nothrow]] static u32 table_internal_should_grow(Table *table_,
                                                       Table_VTable *vtable,
                                                       Allocator *allocator,
                                                       Error **error) {
  Table_Type(byte) *table = table_;
  if (table->length + table->tombs >= table->end - table->end / 4) {
    table_internal_realloc(table, vtable, table->end * 2, allocator, error);
  }
}

[[gnu::nothrow]] static u32 table_insert(Table *table_, Table_VTable *vtable,
                                         void *entry, Allocator *allocator,
                                         Error **error) {
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
    vtable->insert(table->entry + index * vtable->entry_size, entry);
  }
  table->length += 1;
  return index;
}

[[gnu::nothrow]] static u32 table_upsert(Table *table_, Table_VTable *vtable,
                                         void *entry, Allocator *allocator,
                                         Error **error) {
  Table_Type(byte) *table = table_;
  table_internal_should_grow(table, vtable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u32 hash = vtable->hash(entry);
  byte *control = table_internal_control(table, vtable);
  u32 index = table_internal_find(table, vtable, entry, hash);
  if (!(control[index] & TABLE_INTERNAL_CONTROL_ISSET_MASK)) {
    vtable->insert(table->entry + index * vtable->entry_size, entry);
  }
  return index;
}

// TODO: table_find

#endif // TABLE_H_
