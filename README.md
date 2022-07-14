# InverseFX
Computing a function when only its inverse is known, using Newson-Raphson method for 1D,2D,3D arrays in parallel. When user has parallelized version of f(x), it becomes faster. The overall speedup is about 5x for AVX, 5.5x for AVX2 and 7x for AVX512 with x-square function.

Requirements:

- C++14 compiler
- Auto-vectorization of compiler
- Auto-vectorization flags -std=c++14 -O3 -march=native -mavx2 -mprefer-vector-width=256 -ftree-vectorize -fno-math-errno
- Or AVX512 version of flags and 512 vector width

How it works:

- user has an f(x) function given to the constructor
- constructor also takes h(step length) argument for the discrete-derivative calculations such as "two point derivative"/"five point stencil"
- whenever inverse of f(x) at x=x0 is needed, user can call obj.computeInverse...() method
- the computeInverse..() method uses Newton-Raphson method and discrete-derivative to approach inverse f(x0)

Usage:

- parallelized (fast) inversion
```C++
InverseFX::ParallelInverse<float> invPar(
        // user's parallel f(x) function 
        // maps one to one from inp to out, for n elements from first element
        [](float * inp, float * out, int n){
		// C++ compiler vectorizes simple loop easily and possibly inlines this lambda for efficient SIMD
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
      // not as efficient as the parallel f(x) but rest of the algorithm is still parallelized
      [](float inp){ return inp*inp;},
      0.001f /* h */
);

float inp[n],outp[n];

// compute inverse of f(x) at all points (n elements), reading from inp and writing result to outp
invPar.computeInverseLowQuality(inp,outp,n); 
```

- scalar inversion on scalar f(x) function
```C++
InverseFX::ScalarInverse<float> inv([](float inp){
	// black-box function sample
	// f(x)=x*x
	return inp*inp;
},0.001f);

// inverse of x*x is sqrt(x)
float squareRootOfPI = inv.computeInverseLowQuality(3.1415f);
```
