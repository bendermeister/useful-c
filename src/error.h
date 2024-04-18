#ifndef ERROR_H_
#define ERROR_H_

// TODO: documentation
typedef struct Error Error;
struct Error {
  const char *message;
  void *ctx;
};

#endif // ERROR_H_
