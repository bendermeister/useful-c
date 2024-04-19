# Useful C
a small collection of header only libraries useful when coding c

## Basic Ideas
### Error Handling
Error handling is done via `Error` structs. Each function that may error accepts an `Error **err` as their last argument. If `NULL` is passed the functions assume no error is possible and will not check anything and just continue executing. If a pointer is given and an error occurs the pointer is set to the associated global error variable and the user has to check it. A basic example would look like this.

``` c
#include <stdio.h>
#include <uc/allocator.h> // we will come this later

// allocator_alloc will allocate memory according to the passed allocator
// allocator_global is a wrapper around malloc

int main(void) {
    // NULL is passed as the last argument, the function now assumes that it will not fail with the provided input
    int *ptr = allocator_alloc(allocator_global, sizeof(*ptr), NULL);
    
    // now we create a error pointer and the function will no longer invoke UB if it errors
    Error *error = NULL;
    int *ptr2 = allocator_alloc(allocator_global, sizeof(*ptr2), &error);
    if (error) {
       fprintf(stderr, "Error: %s", error->message); 
       
       // check what the error was in this case simple as malloc can only throw one error
       if (error == allocator_global_error) {
           // handle error
       }
    } 
    return 0;
}
```

### Allocator

## Guidelines
- it must compile with `-std=c99 -pedantic`
