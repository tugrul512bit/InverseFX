# InverseFX
Computing a function when only its inverse is known, using Newson-Raphson method for 1D,2D,3D arrays in parallel.

Requirements:

- C++14 compiler
- Auto-vectorization of compiler
- Auto-vectorization flags -std=c++14 -O3 -march=cascadelake -mavx512f -mavx512bw -mprefer-vector-width=512 -ftree-vectorize -fno-math-errno
- Or mavx2/msse for older INTEL CPUs or AMD CPUS
