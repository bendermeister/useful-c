#ifndef TEST_H_
#define TEST_H_

#include <uc/types.h>

#include <stdbool.h>
#include <stdio.h>

static u32 test_total = 0;
static u32 test_passed = 0;

static bool test_print_passed = 0;
static char test_message[1024];

#define TEST_INT(GOT, EXPECTED)                                                \
  test_internal_int(GOT, EXPECTED, __FILE__, __LINE__)

static void test_internal(bool passed, char *file_name, int line,
                          char *fail_message) {
  test_total += 1;
  if (passed) {
    test_passed += 1;
  }

  if (passed && test_print_passed) {
    (void)fprintf(stdout, "%s:%d passed\n", file_name, line);
  }

  if (!passed) {
    (void)fprintf(stderr, "%s:%d failed: %s\n", file_name, line, fail_message);
  }
}

__attribute__((unused)) static void
test_internal_int(i64 got, i64 expected, char *filename, int line) {
  bool passed = got == expected;
  if (!passed) {
    (void)snprintf(test_message, sizeof(test_message), "expected: %ld got: %ld",
                   expected, got);
  }
  test_internal(passed, filename, line, test_message);
}

__attribute__((unused)) static void TEST_OVERVIEW(void) {
  (void)fprintf(stdout,
                "\n\n===============================================\n");
  (void)fprintf(stdout, "\tpassed: % 4d\n", test_passed);
  (void)fprintf(stdout, "\tfailed: % 4d\n", test_total - test_passed);
  (void)fprintf(stdout, "\ttotal:  % 4d\n", test_total);
}

#endif // TEST_H_
