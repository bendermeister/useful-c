#include "../../slice/slice.h"

#include <stdio.h>

int main(void) {
  printf("Hello World!\n");

  int *ints = slice_create(int);

  return 0;
}
