# Useful C
a small collection of C99 header only libraries useful when coding c

## Basic Ideas
### Error Handling
Error handling is done via `Error` values. Each function that may error accepts an `Error *error` as their last argument. If `NULL` is passed the functions assume no error is possible and will not check anything and just continue executing (NOTE: this invokes UB if an error still occurs). If a pointer is given and an error occurs the pointer is set to the associated error value and the user has to check it. A basic example would look like this.

``` c
#include "../../src/error.h"
#include "../../src/types.h"
#include <stdio.h>

// divide may error if b == 0, therefore an error parameter, and an error value
// is provided
//
// NOTE: the function must be able to accept NULL as an error parameter. If NULL
// is provided and an error still occurs the funtion is allowed to enter an
// undefined state
typedef enum { EDIV0 = 1 } ErrorDivision;
i32 divide(i32 a, i32 b, Error *error) {
  // check only if we want to check for errors
  if (error && b == 0) {
    *error = EDIV0;
    return 0;
  }

  return a / b;
}

int main(void) {

  Error error = 0; // error must be initilized for later check

  // if we are not sure a function may error we provide an error parameter
  i32 result = divide(5, 0, &error);

  // now we can handle the error
  if (error) {
    fprintf(stderr, "Error: you can't divide: 5 by 0\n");
  } else {
    fprintf(stdout, "5 divided by 0 is: %d\n", result);
  }

  // if we are sure a function will not error we are allowed to provide a NULL
  // pointer as the error parameter
  result = divide(345, 5, NULL);
  fprintf(stdout, "345 / 5 = %d\n", result);

  return 0;
}
```

### Allocator

## Guidelines
- it must compile with `-std=c99 -pedantic`
