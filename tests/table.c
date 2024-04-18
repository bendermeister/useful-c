#include "../src/table.h"
#include "test.h"

bool int_compare(void *a, void *b, void *ctx) {
  UNUSED(ctx);
  return *(int *)a == *(int *)b;
}

void int_insert(void *dest, void *src, void *ctx) {
  UNUSED(ctx);
  *(int *)dest = *(int *)src;
}

u64 int_hash(void *d, void *ctx) {
  UNUSED(ctx);
  char *s = d;
  u64 hash = 1111111111111111111ul;
  for (int i = 0; i < 4; ++i) {
    hash *= 31;
    hash ^= s[i];
  }
  return hash;
}

static TableVTable vtable = (TableVTable){
    .element_size = sizeof(int),
    .compare = int_compare,
    .insert = int_insert,
    .hash = int_hash,
    .overwrite = int_insert,
};

static void test__insert_find(void) {
  Table(int) table;
  table_init(&table, &vtable, 8, allocator_global, NULL);

  for (int i = 0; i < 1000; ++i) {
    TEST_INT(table.length, i);
    table_insert(&table, &vtable, &i, allocator_global, NULL);
    TEST_INT(table.length, i + 1);
  }

  for (int i = 0; i < 1000; ++i) {
    u32 index = table_find(&table, &vtable, &i);
    u32 isset = table_isset(&table, &vtable, index);
    TEST_INT(isset, 1);
    TEST_INT(table.element[index], i);

    bool contains = table_contains(&table, &vtable, &i);
    TEST_INT(contains, 1);

    int not_contains_entry = i + 1000;
    bool not_contains = table_contains(&table, &vtable, &not_contains_entry);
    TEST_INT(not_contains, 0);
  }

  table_deinit(&table, allocator_global);
}

int main(void) {
  test__insert_find();
  TEST_OVERVIEW();
  return 0;
}
