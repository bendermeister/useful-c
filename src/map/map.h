#ifndef MAP_H_
#define MAP_H_

#include "../allocator/allocator.h"
#include "../error/error.h"
#include "../holy_types/holy_types.h"
#include "../macro_util/macro_util.h"

#include <stdarg.h>
#include <stddef.h>

typedef void Map;

// TODO: do we have to store hashes, storing hashes sucks tremendously

#define Map_Type(KEY_TYPE, VALUE_TYPE)                                         \
  struct {                                                                     \
    KEY_TYPE *key;                                                             \
    VALUE_TYPE *value;                                                         \
    u8 *control;                                                               \
    usize *hash;                                                               \
    usize length;                                                              \
    usize mask;                                                                \
    usize tombs;                                                               \
  }

[[gnu::const]] [[nodiscard]] static unsigned long long
map_next_power_of_two_(unsigned long long x) {
  return 1 << (sizeof(x) * 8 - __builtin_clzll(x));
}

[[nodiscard]] [[gnu::const]] static usize
map_mask_from_capacity_(usize capacity) {
  return map_next_power_of_two_((5 * capacity) / 4) - 1;
}

typedef struct Map_Init_Params_ Map_Init_Params_;
struct Map_Init_Params_ {
  usize initial_capacity;
  Allocator *allocator;
  Error **error;
};

typedef struct Map_Params_ Map_Params_;
struct Map_Params_ {
  Error **error;
  Allocator *allocator;
};

#define map_init(MAP, ...)                                                     \
  ({                                                                           \
    Map_Init_Params_ params_ = {__VA_ARGS__};                                  \
    if (params_.initial_capacity < 16) {                                       \
      params_.initial_capacity = 16;                                           \
    }                                                                          \
    if (!params_.allocator) {                                                  \
      params_.allocator = allocator_global;                                    \
    }                                                                          \
    map_init_(MAP, sizeof(*(MAP)->key), sizeof(*(MAP)->value), params_);       \
  })

[[maybe_unused]] static void map_init_(void *map_, usize key_size,
                                       usize value_size,
                                       Map_Init_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  map->mask = map_mask_from_capacity_(params.initial_capacity);

  usize chunk_size = map->mask + 1;
  chunk_size += (map->mask + 1) * key_size;
  chunk_size += (map->mask + 1) * value_size;
  chunk_size += (map->mask + 1) * sizeof(*map->hash);

  map->control =
      (u8 *)allocator_alloc(params.allocator, chunk_size, params.error);
  if (UNLIKELY(params.error && *params.error)) {
    return;
  }
  __builtin_memset(map->control, 0, map->mask + 1);

  map->key = map->control + map->mask + 1;
  map->value = map->key + (map->mask + 1) * key_size;
  map->hash = (usize *)(map->value + (map->mask + 1) * value_size);
  map->tombs = 0;
  map->length = 0;
}

#define map_deinit(MAP, ...)                                                   \
  ({                                                                           \
    Map_Params_ params = {__VA_ARGS__};                                        \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    map_deinit_(MAP, params);                                                  \
  })

[[maybe_unused]] static void map_deinit_(void *map_, Map_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  allocator_free(params.allocator, map->control, params.error);
}

constexpr u8 MAP_ISSET_MASK = 1 << 7;

[[maybe_unused]] static usize map_find_start(void *map_, usize hash) {
  Map_Type(byte, byte) *map = map_;
  return hash & map->mask;
}

[[gnu::const]] [[maybe_unused]] static u8 map_control_hash_(usize hash) {
  return (hash & 255) | MAP_ISSET_MASK;
}

[[gnu::hot]] [[maybe_unused]] static usize map_find(void *map_, usize hash,
                                                    usize i) {
  Map_Type(byte, byte) *map = map_;
  i += 1;
  i &= map->mask;

  u8 control_hash = map_control_hash_(hash);

  while (1) {
    if (map->control[i] == 0) {
      return i;
    }
    if (map->control[i] == control_hash && map->hash[i] == hash) {
      return i;
    }
    i += 1;
    i &= map->mask;
  }
}

[[maybe_unused]] static void map_insert(void *map_, usize hash, usize i) {
  Map_Type(byte, byte) *map = map_;
  map->length += 1;
  map->control[i] = map_control_hash_(hash);
  map->hash[i] = hash;
}

[[maybe_unused]] static void map_realloc_(void *map_, usize key_size,
                                          usize value_size, usize new_mask,
                                          Map_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  Map_Type(byte, byte) map_new;

  map_init_(&map_new, key_size, value_size,
            (Map_Init_Params_){
                .initial_capacity = new_mask,
                .allocator = params.allocator,
                .error = params.error,
            });
  if (UNLIKELY(params.error && *params.error)) {
    return;
  }

  for (usize i = 0; i <= map->mask; ++i) {
    if (!(MAP_ISSET_MASK & map->control[i])) {
      continue;
    }
    usize j =
        map_find(&map_new, map->hash[i], map_find_start(map, map->hash[i]));
    while (MAP_ISSET_MASK & map_new.control[j]) {
      j = map_find(&map_new, map->hash[i], j);
    }

    __builtin_memcpy(&map_new.key[j * key_size], &map->key[i * key_size],
                     key_size);
    __builtin_memcpy(&map_new.value[j * value_size],
                     &map->value[i * value_size], value_size);
    map_insert(&map_new, map->hash[i], j);
  }

  map_deinit_(map, params);
  if (UNLIKELY(params.error && *params.error)) {
    map_deinit_(&map_new, params);
    return;
  }
  __builtin_memcpy(map, &map_new, sizeof(*map));
}

[[maybe_unused]] static void map_remove(void *map_, usize i) {
  Map_Type(byte, byte) *map = map_;
  map->length -= 1;
  map->tombs += 1;
  map->control[i] = 1;
}

#define map_prepare_insert(MAP, ...)                                           \
  ({                                                                           \
    Map_Params_ params = {__VA_ARGS__};                                        \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    map_prepare_insert_(MAP, sizeof(*(MAP)->key), sizeof(*(MAP)->value),       \
                        params);                                               \
  })

[[maybe_unused]] static void map_prepare_insert_(void *map_, usize key_size,
                                                 usize value_size,
                                                 Map_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  if (map->length + map->tombs >= map->mask - map->mask / 4) {
    map_realloc_(map, key_size, value_size, map->mask * 2, params);
  }
}

[[maybe_unused]] static bool map_isset(void *map_, usize i) {
  Map_Type(byte, byte) *map = map_;
  return map->control[i] & MAP_ISSET_MASK;
}

#define map_reserve(MAP, NEW_CAPACITY, ...)                                    \
  ({                                                                           \
    Map_Params_ params = {__VA_ARGS__};                                        \
    if (!params.allocator) {                                                   \
      params.allocator = allocator_global;                                     \
    }                                                                          \
    map_reserve_(MAP, sizeof(*(MAP)->key), sizeof(*(MAP)->value),              \
                 NEW_CAPACITY, params);                                        \
  })

[[maybe_unused]] static void map_reserve_(void *map_, usize key_size,
                                          usize value_size, usize new_capacity,
                                          Map_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  usize new_mask = map_mask_from_capacity_(new_capacity);
  if (map->mask >= new_mask) {
    return;
  }
  map_realloc_(map, key_size, value_size, new_mask, params);
}

[[maybe_unused]] static void map_shrink_(void *map_, usize key_size,
                                         usize value_size, Map_Params_ params) {
  Map_Type(byte, byte) *map = map_;
  usize new_mask = map_mask_from_capacity_(map->length);
  if (map->mask != new_mask) {
    map_realloc_(map, key_size, value_size, new_mask, params);
  }
}

#endif // MAP_H_
