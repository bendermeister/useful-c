#ifndef ERROR_H_
#define ERROR_H_

#include "../holy_types/holy_types.h"

typedef struct Error Error;
struct Error {
  void *ctx;
  usize length;
  char *message;
};

#define ERROR_MAKE(STR)                                                        \
  (Error) { .length = sizeof(STR) - 1, .message = STR, }

#endif // ERROR_H_
