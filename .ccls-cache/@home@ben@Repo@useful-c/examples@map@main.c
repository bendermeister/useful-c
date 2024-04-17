#include <stdio.h>

#include "../../map/map.h"

#include <assert.h>

int main(void) {
  printf("Hello World!\n");

  Map_Type(int, int) map;

  map_init(&map);

  for (int i = 0; i < 100; ++i) {
    map_prepare_insert(&map);
    usize pos = map_find(&map, i, map_find_start(&map, i));
    assert(!map_isset(&map, pos));
    map_insert(&map, i, pos);
    map.key[pos] = i;
    map.value[pos] = i + 69;
  }

  for (usize pos = 0, i = 0; i < map.length; ++i) {
    while (!map_isset(&map, pos)) {
      pos += 1;
    }
    printf("%d: %d\n", map.key[pos], map.value[pos]);
    pos += 1;
  }

  map_deinit(&map);

  return 0;
}
