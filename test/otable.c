#include "test.h"
#include <uc/otable.h>

static bool int_compare(const void *a, const void *b, void *ctx) {
  UNUSED(ctx);
  return *(const int *)a == *(const int *)b;
}

static void int_insert(void *dest, const void *src, void *ctx) {
  UNUSED(ctx);
  *(int *)dest = *(const int *)src;
}

static u64 int_hash(const void *d, void *ctx) {
  UNUSED(ctx);
  const char *s = d;
  u64 hash = 1111111111111111111ul;
  for (int i = 0; i < 4; ++i) {
    hash *= 31;
    hash ^= s[i];
  }
  return hash;
}

static const OTableVTable vtable = (OTableVTable){
    .element_size = sizeof(int),
    .compare = &int_compare,
    .insert = &int_insert,
    .hash = &int_hash,
    .overwrite = &int_insert,
};

static void test__insert_find(void) {
  OTable(int) table;
  otable_init(&table, &vtable, 8, allocator_global, NULL);

  // testing insert and vtable->insert
  for (int i = 0; i < 1000; ++i) {
    TEST_INT(table.length, i);
    otable_insert(&table, &vtable, &i, allocator_global, NULL);
    TEST_INT(table.length, i + 1);
  }

  // testing find and contains
  for (int i = 0; i < 1000; ++i) {
    u32 index = otable_find(&table, &vtable, &i);
    u32 isset = otable_isset(&table, &vtable, index);
    TEST_INT(isset, 1);
    TEST_INT(table.element[index], i);

    bool contains = otable_contains(&table, &vtable, &i);
    TEST_INT(contains, 1);

    int not_contains_entry = i + 1000;
    bool not_contains = otable_contains(&table, &vtable, &not_contains_entry);
    TEST_INT(not_contains, 0);
  }

  // testing upsert
  for (int i = 0; i < 1000; ++i) {
    usize index = otable_upsert(&table, &vtable, &i, allocator_global, NULL);
    bool contains = otable_isset(&table, &vtable, index);
    TEST_INT(contains, 1);
    TEST_INT(table.element[index], i);
  }

  otable_deinit(&table, allocator_global);
}

int main(void) {
  test__insert_find();
  TEST_OVERVIEW();
  return 0;
}
