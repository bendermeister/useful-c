#ifndef MACRO_UTIL_H_
#define MACRO_UTIL_H_

#define UNUSED(A) (void)(A)
#define LIKELY(A) (__builtin_expect((A), 1))
#define UNLIKELY(A) (__builtin_expect((A), 0))
#define TRAP() __builtin_trap()

#endif // MACRO_UTIL_H_
