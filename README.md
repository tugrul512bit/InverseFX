# InverseFX
Computing a function when only its inverse is known, using Newson-Raphson method for 1D,2D,3D arrays in parallel. When user has parallelized version of f(x), it becomes faster. The overall speedup is about 5x for AVX, 5.5x for AVX2 and 7x for AVX512 with x-square function.

Requirements:

- C++14 compiler
- Auto-vectorization of compiler
- Auto-vectorization flags -std=c++14 -O3 -march=native -mavx2 -mprefer-vector-width=256 -ftree-vectorize -fno-math-errno
- Or AVX512 version of flags and 512 vector width
