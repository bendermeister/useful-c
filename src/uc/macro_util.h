#ifndef MACRO_UTIL_H_
#define MACRO_UTIL_H_

#include "builtin.h"

#define UNUSED(A) ((void)A)

#define LIKELY(A) builtin_expect((A), 1)
#define UNLIKELY(A) builtin_expect((A), 0)

#endif // MACRO_UTIL_H_
