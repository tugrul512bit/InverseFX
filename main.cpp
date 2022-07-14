

#include<iostream>
#include"InverseFX.h"
int main()
{
	// scalar test
	InverseFX::ScalarInverse<float> inv([](float inp){
		// black-box function sample
		// f(x)=x*x
		return inp*inp;
	},0.01f);
	std::cout<<inv.computeInverseLowQuality(3.1415f)<<std::endl;

	// scalar on array test: slow
	constexpr int n = 67353;
	float arr[n];


	size_t t;
	for(int j=0;j<20;j++)
	{
		// prepare input
		for(int i=0;i<n;i++)
		{
			arr[i]=i+1;
		}

		// benchmark
		{

			InverseFX::Bench bench(&t);

			for(int i=0;i<n;i++)
			{
				arr[i]=inv.computeInverseLowQuality(arr[i]);
			}
		}
		std::cout<< n/(double)t << " elements per nanosecond"<<std::endl;
	}

	float acc = 0;
	for(int i=0;i<n;i++)
	{
		acc +=arr[i];
	}
	std::cout<<acc<<std::endl;

	// parallelized version
	{
		InverseFX::ParallelInverse<float> invPar([](float inp){
			return inp*inp;
		},0.01f);

		float inp[n],outp[n];
		for(int j=0;j<20;j++)
		{
			// prepare input
			for(int i=0;i<n;i++)
			{
				inp[i]=i+1;
			}

			// benchmark
			{
				InverseFX::Bench bench(&t);
				invPar.computeInverseLowQuality(inp,outp,n);
			}
			std::cout<< n/(double)t << " elements per nanosecond"<<std::endl;
		}

		acc = 0;
		for(int i=0;i<n;i++)
		{
			acc +=outp[i];
		}
		std::cout<<acc<<std::endl;
	}
	// parallelized version with parallel f(x)
	{
		InverseFX::ParallelInverse<float> invPar([](float * inp, float * out, int n){
			for(int i=0;i<n;i++)
			{
				out[i]=inp[i]*inp[i];
			}

		},0.01f);

		float inp[n],outp[n];
		for(int j=0;j<20;j++)
		{
			// prepare input
			for(int i=0;i<n;i++)
			{
				inp[i]=i+1;
			}

			// benchmark
			{
				InverseFX::Bench bench(&t);
				invPar.computeInverseLowQuality(inp,outp,n);
			}
			std::cout<< n/(double)t << " elements per nanosecond"<<std::endl;
		}

		acc = 0;
		for(int i=0;i<n;i++)
		{
			acc +=outp[i];
		}
		std::cout<<acc<<std::endl;
	}
	return 0;
}


/*
scalar test: 1.77243 (square root of pi)

scalar on array test (fx8150 @ 2.1 GHz):
0.00211415 elements per nanosecond
0.00223072 elements per nanosecond
0.00223016 elements per nanosecond
0.00223383 elements per nanosecond
0.00223186 elements per nanosecond
0.00223494 elements per nanosecond
0.0021805 elements per nanosecond
0.00210438 elements per nanosecond
0.00215071 elements per nanosecond
0.00214863 elements per nanosecond
0.0021917 elements per nanosecond
0.00219616 elements per nanosecond
0.00218305 elements per nanosecond
0.00218948 elements per nanosecond
0.00221218 elements per nanosecond
0.00221851 elements per nanosecond
0.0021991 elements per nanosecond
0.00221038 elements per nanosecond
0.00215757 elements per nanosecond
0.00218085 elements per nanosecond
1.1185e+07

parallelized version without parallel f(x) (fx8150 @ 2.1 GHz):
0.00453103 elements per nanosecond
0.00458556 elements per nanosecond
0.00483413 elements per nanosecond
0.0048587 elements per nanosecond
0.00484984 elements per nanosecond
0.00485454 elements per nanosecond
0.00480151 elements per nanosecond
0.00476971 elements per nanosecond
0.00478836 elements per nanosecond
0.00487069 elements per nanosecond
0.00487621 elements per nanosecond
0.00487004 elements per nanosecond
0.00487827 elements per nanosecond
0.00487663 elements per nanosecond
0.00463526 elements per nanosecond
0.00477984 elements per nanosecond
0.00477555 elements per nanosecond
0.00481585 elements per nanosecond
0.0047672 elements per nanosecond
0.0048717 elements per nanosecond
1.1185e+07

parallelized version with parallelized f(x) (fx8150 @ 2.1GHz)
0.00941158 elements per nanosecond
0.0101714 elements per nanosecond
0.0113167 elements per nanosecond
0.0113237 elements per nanosecond
0.0113976 elements per nanosecond
0.0113068 elements per nanosecond
0.0113352 elements per nanosecond
0.0114023 elements per nanosecond
0.0113305 elements per nanosecond
0.0112831 elements per nanosecond
0.0113745 elements per nanosecond
0.011416 elements per nanosecond
0.011405 elements per nanosecond
0.0114099 elements per nanosecond
0.0113726 elements per nanosecond
0.0114159 elements per nanosecond
0.0114166 elements per nanosecond
0.0113632 elements per nanosecond
0.0113744 elements per nanosecond
0.0113766 elements per nanosecond
1.1185e+07

~5x speedup with FX8150's AVX

*/

// With godbolt.org's server that has AVX2 (and relevant compiler flags enabled)
// 0.00575837 elements per nanosecond ---> scalar
// 0.0121961 elements per nanosecond ----> parallel run with only scalar f(x) given for constructor
// 0.0341529 elements per nanosecond ----> fully parallel (~6x speedup)

// With AVX512 CPU of godbolt.org server (and avx512f flag enabled with preferred vector width=512)
// 0.0403193 elements per nanosecond ----> fully parallel with lower frequency than AVX2
