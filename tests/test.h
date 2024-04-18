#ifndef TEST_H_
#define TEST_H_

#include "../src/holy_types/holy_types.h"

#include <stdbool.h>
#include <stdio.h>

static u32 test_total = 0;
static u32 test_passed = 0;

static bool test_print_passed = 0;
static char test_message[1024];

#define TEST_INT(GOT, EXPECTED)                                                \
  ({                                                                           \
    i64 test_got = GOT;                                                        \
    i64 test_expected = EXPECTED;                                              \
    bool test_passed = GOT == EXPECTED;                                        \
    if (!test_passed) {                                                        \
      snprintf(test_message, sizeof(test_message), "expected: %ld got: %ld",   \
               test_expected, test_got);                                       \
    }                                                                          \
    test_internal(test_passed, __FILE__, __LINE__, (char *)__func__,           \
                  test_message);                                               \
  })

static void test_internal(bool passed, char *file_name, int line,
                          char *function, char *fail_message) {
  test_total += 1;
  if (passed) {
    test_passed += 1;
  }

  if (passed && test_print_passed) {
    fprintf(stdout, "%s:%d passed\n", file_name, line);
  }

  if (!passed) {
    fprintf(stderr, "%s:%d %s failed: %s\n", file_name, line, function,
            fail_message);
  }
}

static void TEST_OVERVIEW() {
  fprintf(stdout, "\n\n===============================================\n");
  fprintf(stdout, "\tpassed: % 4d\n", test_passed);
  fprintf(stdout, "\tfailed: % 4d\n", test_total - test_passed);
  fprintf(stdout, "\ttotal:  % 4d\n", test_total);
}

#endif // TEST_H_
