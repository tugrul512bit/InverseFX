
#include<iostream>
#include"InverseFX.h"
int main()
{
	// scalar test
	InverseFX::ScalarInverse<float> inv([](float inp){
		// black-box function sample
		// f(x)=x*x
		return inp*inp;
	},0.001f);
	std::cout<<inv.computeInverseLowQuality(3.1415f)<<std::endl;

	// scalar on array test: slow
	constexpr int n = 15000;
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
		},0.001f);

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

		},0.001f);

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
1.77243 -----> square root (inverse of x*x) of 3.1415

scalar work on array:
0.00248134 elements per nanosecond
0.00234355 elements per nanosecond
0.00246709 elements per nanosecond
0.00249108 elements per nanosecond
0.00247686 elements per nanosecond
0.00249032 elements per nanosecond
0.00249328 elements per nanosecond
0.00247671 elements per nanosecond
0.00248345 elements per nanosecond
0.00248592 elements per nanosecond
0.0024748 elements per nanosecond
0.00248991 elements per nanosecond
0.00249236 elements per nanosecond
0.00249008 elements per nanosecond
0.00237878 elements per nanosecond
0.00247678 elements per nanosecond
0.00249626 elements per nanosecond
0.00248505 elements per nanosecond
0.00248557 elements per nanosecond
0.00249246 elements per nanosecond
1.22481e+06

parallel work on array with only scalar f(x) given by user:
0.00516736 elements per nanosecond
0.00519919 elements per nanosecond
0.00521338 elements per nanosecond
0.0052303 elements per nanosecond
0.00524147 elements per nanosecond
0.00520205 elements per nanosecond
0.00521525 elements per nanosecond
0.00524055 elements per nanosecond
0.00520077 elements per nanosecond
0.00523028 elements per nanosecond
0.00517554 elements per nanosecond
0.00524118 elements per nanosecond
0.00521376 elements per nanosecond
0.00520089 elements per nanosecond
0.00524106 elements per nanosecond
0.00519718 elements per nanosecond
0.00519525 elements per nanosecond
0.00521436 elements per nanosecond
0.00524124 elements per nanosecond
0.00517988 elements per nanosecond
1.22481e+06

parallel work on array with parallel f(x) given by user:
0.0114558 elements per nanosecond
0.0110267 elements per nanosecond
0.0112143 elements per nanosecond
0.0112153 elements per nanosecond
0.0111488 elements per nanosecond
0.0109558 elements per nanosecond
0.0115662 elements per nanosecond
0.0113101 elements per nanosecond
0.0115026 elements per nanosecond
0.0115608 elements per nanosecond
0.0113773 elements per nanosecond
0.0115689 elements per nanosecond
0.0115629 elements per nanosecond
0.011143 elements per nanosecond
0.0112179 elements per nanosecond
0.0112201 elements per nanosecond
0.01098 elements per nanosecond
0.0112175 elements per nanosecond
0.0112157 elements per nanosecond
0.0110193 elements per nanosecond
1.22481e+06

~5.5x speedup with FX8150's AVX

*/

// With godbolt.org's server that has AVX2 (and relevant compiler flags enabled)
// 0.00575837 elements per nanosecond ---> scalar
// 0.0121961 elements per nanosecond ----> parallel run with only scalar f(x) given for constructor
// 0.0341529 elements per nanosecond ----> fully parallel (~6x speedup)

// With AVX512 CPU of godbolt.org server (and avx512f flag enabled with preferred vector width=512)
// 0.0403193 elements per nanosecond ----> fully parallel with lower frequency than AVX2 (~7x speedup depending on server-load)
