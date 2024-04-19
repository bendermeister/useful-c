#ifndef DEBUG_CHECK_H_
#define DEBUG_CHECK_H_

#include "builtin.h"

#ifdef DEBUG
#define debug_check(A)                                                         \
  if (!(A))                                                                    \
  builtin_trap()
#else
#define debug_check(A)
#endif

#endif // DEBUG_CHECK_H_
