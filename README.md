# InverseFX
Computing a function when only its inverse is known, using Newson-Raphson method for 1D,2D,3D arrays in parallel. When user has parallelized version of f(x), it becomes faster. The overall speedup is about 5x for AVX, 5.5x for AVX2 and 7x for AVX512 with x-square function.

Requirements:

- C++14 compiler
- Auto-vectorization of compiler
- Auto-vectorization flags -std=c++14 -O3 -march=native -mavx2 -mprefer-vector-width=256 -ftree-vectorize -fno-math-errno
- Or AVX512 version of flags and 512 vector width

Usage:

- parallelized (fast) inversion
```C++
InverseFX::ParallelInverse<float> invPar(
        // user's parallel f(x) function (C++ compiler vectorizes simple loop easily)
        // maps one to one from inp to out, for n elements from first element
        [](float * inp, float * out, int n){
			    for(int i=0;i<n;i++)
			    {
				    out[i]=inp[i]*inp[i];
			    }
		    },
        // h value that is used for computing two-point derivative inside the inversion logic
        0.001f
);

float inp[n],outp[n];

// compute inverse of f(x) at all points (n elements), reading from inp and writing result to outp
invPar.computeInverseLowQuality(inp,outp,n); 
```

- parallelized inversion using scalar f(x) function
```C++
InverseFX::ParallelInverse<float> invPar(
      // f(x)=x*x, for answering question of "what is inverse of x*x?"
      [](float inp){ return inp*inp;},
      0.001f /* h */
);

float inp[n],outp[n];

// compute inverse of f(x) at all points (n elements), reading from inp and writing result to outp
invPar.computeInverseLowQuality(inp,outp,n); 
```
