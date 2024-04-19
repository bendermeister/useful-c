# To Be Implemented
## Vector
A simple vector implementation in the same style as table

## simd
A wrapper around the simd compiler intrinsics which provides fallbacks for architectures without the associated simd instruction
- type
  - v16u8: to indicate vector of 16 unisgned 8 bit integers
  - v128: to indicate vector of 128 bits
  
## arena allocator
- a simple arena allocator just backed by a user provided array
- a backed arena allocator which first uses up a user provided array and then asks a user provided allocator for more shit
  - this will need some sort of null_allocator which just always fails

## builtin
A wrapper around gcc and clang builtin funtion with fallbacks for compilers without the associated builtins

## concurrency
- some concurrency model, 
- I quite like the go concurrency model and it would be cool to port it to c
- it should be some sort of green threads implementation and not acutal hardware threads

## btree
- some btree implementation maybe even a b* with actual file backing would be pretty sweet
