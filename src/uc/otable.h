#ifndef OTABLE_H_
#define OTABLE_H_

#include "allocator.h"
#include "builtin.h"
#include "debug_check.h"
#include "error.h"
#include "types.h"

#include <emmintrin.h>

/***
 * @doc(type): otable_element_insert_f
 * @tag: all
 *
 * @brief: Callback for inserting elements into the otable.
 *
 * @detailed: This callback is used by `otable_insert` and `otable_upsert` when
 * inserting an element into the otable, which is not already present.
 * `otable_element_insert_f` is required to prepare `dest` in such a way that
 * the `dest` element can later be identified via `otable_element_compare_f`.
 * You can think of this function as the constructor of the elements in the
 * otable.
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
 * inside the votable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*otable_element_insert_f)(void *dest, const void *src, void *ctx);

/**
 * @doc(type): otable_element_overwrite_f
 * @tag: all
 *
 * @brief: Callback for overwriting elements in the otable.
 *
 * @detailed: This callback is used by `otable_insert` to overwrite elements in
 * the otable which are already present. `otable_element_overwrite_f` is
 * required to leave `dest` in such a state that it can later be identified by
 * `otable_element_compare_f`
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
 * inside the votable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*otable_element_overwrite_f)(void *dest, const void *src,
                                           void *ctx);

/**
 * @doc(type): otable_element_destroy_f
 * @tag: all
 *
 * @brief: Callback for destroying elements in the otable.
 *
 * @detailed: Callback for destoying elements in the otable. It will be called
 * by `otable_remove` on the removed element and by `otable_destroy` on each
 * element. It may be `NULL` in which case it will never get called.
 *
 * @param(element): element which is to be destroyed
 * @assert(element): `element != NULL`
 * @assert(element): element will always contain a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the votable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef void (*otable_element_destroy_f)(void *element, void *ctx);

/**
 * @doc(type): otable_element_compare_f
 * @tag: all
 *
 * @brief: Callback used for identifying elements in the otable
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
 * inside the votable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef bool (*otable_element_compare_f)(const void *first, const void *second,
                                         void *ctx);

/***
 * @doc(type): otable_element_hash_f
 * @tag: all
 *
 * @brief: Callback used for hashing elements in the otable.
 *
 * @param(element): element which is to be hashed
 * @assert(element): `element != NULL`
 * @assert(element): element will always point to a valid memory address
 *
 * @param(ctx): context pointer which will be passed from the context pointer
 * inside the votable to provide additional information to the function, and
 * prevent use of global variables
 */
typedef u64 (*otable_element_hash_f)(const void *element, void *ctx);

/***
 * @doc(type): OTableVTable
 * @tag: all
 *
 * @brief: Struct for generically handling elements in the otable.
 *
 * @detailed: Struct which provides function pointers to the otable which it
 * then uses internally to manage its elements. Each type is described
 * seperately.
 *
 * @member(element_size): `sizeof(element)`. it should represent the object size
 * of each element in bytes.
 * @assert(element_size): `element_size > 0`
 *
 * @member(destroy): callback for destroying elements. `destoy` may be `NULl` in
 * which case no destructor will be called. This will make `otable_deinit`
 * significantly faster
 *
 * @member(hash): callback for hashing elements
 * @assert(hash): `hash != NULL`
 *
 * @member(insert): callback for inserting elements into the otable. Can be
 * thought of as the constructor of elements in the otable
 * @assert(insert): `insert != NULL`
 *
 * @member(compare): callback for comparing elements inside the otable
 * @assert(compare): `compare != NULL`
 *
 * @member(overwrite): callback for overwriting elements inside the otable.
 * Useful for maps which need to update the value but not the key
 * @assert(overwrite): `overwrite != NULL`
 *
 * @member(ctx): context pointer passed to each callback. May be `NULL`.
 */
typedef struct OTableVTable OTableVTable;
struct OTableVTable {
  otable_element_hash_f hash;
  otable_element_insert_f insert;
  otable_element_compare_f compare;
  otable_element_destroy_f destroy;
  otable_element_overwrite_f overwrite;
  usize element_size;
  void *ctx;
};

/**
 * @doc(type): OTable
 * @tag: all
 *
 * @brief Opaque pointer to a otable struct.
 *
 * @detailed: Opaque pointer to a otable struct.
 * @assert: otable struct has to follow the form `OTable(TYPE)`
 */
typedef void OTable;

/***
 * @doc(type): OTable(TYPE)
 * @tag: all
 *
 * @brief struct type for a otable with elements of type `TYPE`
 *
 * @param(TYPE)
 */
#define OTable(TYPE)                                                           \
  struct {                                                                     \
    TYPE *element;                                                             \
    usize length;                                                              \
    usize end;                                                                 \
    usize tombs;                                                               \
  }

// TODO: documentation
#define OTABLE_INTERNAL_CONTROL_ISSET_MASK ((u8)(1 << 7))
#define OTABLE_INTERNAL_CONTROL_TOMB ((u8)1)
#define OTABLE_INTERNAL_CONTROL_FREE ((u8)0)

// TODO: documentation
static u8 otable_internal_hash_to_control_byte(u64 hash) {
  return (hash & 255) | OTABLE_INTERNAL_CONTROL_ISSET_MASK;
}

// TODO: documentation
static usize otable_internal_end_from_capacity(usize capacity) {
  debug_check(capacity > 0);
  capacity *= 8;
  capacity /= 7;
  return 1 << (8 * sizeof(unsigned long long) -
               builtin_clzll((unsigned long long)capacity));
}

// TODO: documentation
// TODO: assert
static byte *otable_internal_control_array(const OTable *otable_,
                                           const OTableVTable *votable) {
  debug_check(otable_);
  debug_check(votable);

  const OTable(byte) *otable = otable_;
  return otable->element + votable->element_size * otable->end;
}

// TODO: documentation
// TODO: assert
static void otable_deinit(OTable *otable_, Allocator *allocator) {
  debug_check(otable_);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  allocator_free(allocator, otable->element);
}

// TODO: documentation
// TODO: assert
static void otable_internal_init(OTable *otable_, const OTableVTable *votable,
                                 usize end, Allocator *allocator,
                                 Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(end > 0); // assert end is power of two
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  builtin_memset(otable, 0, sizeof(*otable));
  otable->end = end;

  usize chunk_size = votable->element_size * otable->end;
  chunk_size += otable->end + 16;

  otable->element = allocator_alloc(allocator, chunk_size, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  byte *control = otable_internal_control_array(otable, votable);
  builtin_memset(control, OTABLE_INTERNAL_CONTROL_FREE, 16 + otable->end);
}

static void otable_init(OTable *otable, const OTableVTable *votable,
                        usize initial_capacity, Allocator *allocator,
                        Error *error) {
  // TODO: this way if end wil always be over 16 this should not be the case if
  // initial capacity can be satisfied with end == 16
  if (initial_capacity < 16) {
    initial_capacity = 16;
  }
  usize end = otable_internal_end_from_capacity(initial_capacity);
  otable_internal_init(otable, votable, end, allocator, error);
}

static usize otable_internal_find(const OTable *otable_,
                                  const OTableVTable *votable,
                                  const void *element, u64 hash) {
  // TODO: quadratic probing
  debug_check(otable_);
  debug_check(votable);
  debug_check(element);

  const OTable(byte) *otable = otable_;
  const usize mask = otable->end - 1;
  usize index = hash & mask;
  const __m128i control_mask =
      _mm_set1_epi8(otable_internal_hash_to_control_byte(hash));
  const __m128i zero = _mm_set1_epi8(0);
  const byte *control = otable_internal_control_array(otable, votable);

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

        if (votable->compare(
                element, &otable->element[votable->element_size * real_index],
                votable->ctx)) {
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

static usize otable_find(const OTable *otable, const OTableVTable *votable,
                         const void *element) {
  debug_check(otable);
  debug_check(votable);
  debug_check(element);

  return otable_internal_find(otable, votable, element,
                              votable->hash(element, votable->ctx));
}

static bool otable_isset(const OTable *otable, const OTableVTable *votable,
                         usize index) {
  debug_check(otable);
  debug_check(votable);
  debug_check(((OTable(byte) *)otable)->end > index);

  return otable_internal_control_array(otable, votable)[index] &
         OTABLE_INTERNAL_CONTROL_ISSET_MASK;
}

// TODO: otable_remove

static bool otable_contains(const OTable *otable_, const OTableVTable *votable,
                            const void *element) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(element);

  usize index = otable_find(otable_, votable, element);
  return otable_isset(otable_, votable, index);
}

static void otable_internal_realloc(OTable *otable_,
                                    const OTableVTable *votable, usize end,
                                    Allocator *allocator, Error *error) {

  debug_check(otable_);
  debug_check(votable);
  debug_check(end > 0); // TODO: check if end is power of two
  debug_check(allocator);

  // TODO: does this increase capacity more than it should?
  OTable(byte) *otable = otable_;

  OTable(byte) otable_new;

  otable_internal_init(&otable_new, votable, end, allocator, error);
  if (UNLIKELY(error && *error)) {
    return;
  }

  otable_new.length = otable->length;

  byte *otable_new_control =
      otable_internal_control_array(&otable_new, votable);
  byte *otable_control = otable_internal_control_array(otable, votable);

  for (usize i = 0; i < otable->end; ++i) {
    if (UNLIKELY(!(otable_control[i] & OTABLE_INTERNAL_CONTROL_ISSET_MASK))) {
      continue;
    }

    byte *element = otable->element + votable->element_size * i;
    const u64 hash = votable->hash(element, votable->ctx);
    const usize j = otable_internal_find(&otable_new, votable, element, hash);
    builtin_memcpy(otable_new.element + votable->element_size * j, element,
                   votable->element_size);
    otable_new_control[j] = otable_internal_hash_to_control_byte(hash);
    if (UNLIKELY(j < 16)) {
      otable_new_control[j + otable_new.end] =
          otable_internal_hash_to_control_byte(hash);
    }
  }

  allocator_free(allocator, otable->element);
  builtin_memcpy(otable, &otable_new, sizeof(*otable));
}

static void otable_shrink(OTable *otable_, const OTableVTable *votable,
                          Allocator *allocator, Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  usize end = otable_internal_end_from_capacity(otable->length);
  otable_internal_realloc(otable, votable, end, allocator, error);
}

static void otable_reserve(OTable *otable_, const OTableVTable *votable,
                           usize capacity, Allocator *allocator, Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(capacity > 0);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  usize end = otable_internal_end_from_capacity(capacity);
  if (end <= otable->end) {
    return;
  }

  otable_internal_realloc(otable, votable, end, allocator, error);
}

static void otable_internal_should_grow(OTable *otable_,
                                        const OTableVTable *votable,
                                        Allocator *allocator, Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  if (otable->length + otable->tombs >= otable->end - otable->end / 8) {
    otable_internal_realloc(otable, votable, otable->end * 2, allocator, error);
  }
}

static void otable_internal_insert(OTable *otable_, const OTableVTable *votable,
                                   const void *element, u64 hash, usize index) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(element);
  debug_check(((OTable(byte) *)otable_)->end > index);

  OTable(byte) *otable = otable_;
  byte *control = otable_internal_control_array(otable, votable);
  control[index] = otable_internal_hash_to_control_byte(hash);
  if (index < 16) {
    control[index + otable->end] = otable_internal_hash_to_control_byte(hash);
  }
  votable->insert(otable->element + index * votable->element_size, element,
                  votable->ctx);
  otable->length += 1;
}

static usize otable_insert(OTable *otable_, const OTableVTable *votable,
                           const void *element, Allocator *allocator,
                           Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(element);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  otable_internal_should_grow(otable, votable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u64 hash = votable->hash(element, votable->ctx);
  usize index = otable_internal_find(otable, votable, element, hash);

  byte *control = otable_internal_control_array(otable, votable);

  if (control[index] & OTABLE_INTERNAL_CONTROL_ISSET_MASK) {
    votable->overwrite(otable->element + index * votable->element_size, element,
                       votable->ctx);
  } else {
    otable_internal_insert(otable, votable, element, hash, index);
  }
  return index;
}

static usize otable_upsert(OTable *otable_, const OTableVTable *votable,
                           const void *element, Allocator *allocator,
                           Error *error) {
  debug_check(otable_);
  debug_check(votable);
  debug_check(element);
  debug_check(allocator);

  OTable(byte) *otable = otable_;
  otable_internal_should_grow(otable, votable, allocator, error);
  if (UNLIKELY(error && *error)) {
    return -1;
  }

  const u64 hash = votable->hash(element, votable->ctx);
  byte *control = otable_internal_control_array(otable, votable);
  usize index = otable_internal_find(otable, votable, element, hash);
  if (!(control[index] & OTABLE_INTERNAL_CONTROL_ISSET_MASK)) {
    otable_internal_insert(otable, votable, element, hash, index);
  }
  return index;
}

// NOTE: this is really stupid but we need to get rid of unwanted unused
// warnings without attributes
static void _otable_dummy_callee__(void);
static void _otable_dummy_caller__(void) {
  otable_init(NULL, NULL, 0, NULL, NULL);
  otable_deinit(NULL, NULL);

  otable_insert(NULL, NULL, NULL, NULL, NULL);
  otable_upsert(NULL, NULL, NULL, NULL, NULL);

  otable_reserve(NULL, NULL, 0, NULL, NULL);
  otable_shrink(NULL, NULL, NULL, NULL);

  otable_find(NULL, NULL, NULL);
  otable_isset(NULL, NULL, 0);
  otable_contains(NULL, NULL, NULL);
  _otable_dummy_callee__();
}
static void _otable_dummy_callee__(void) { _otable_dummy_caller__(); }

#endif // OTABLE_H_
