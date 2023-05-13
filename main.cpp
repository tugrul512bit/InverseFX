
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
1.77243 --> inverse of x*x (sqrt(x)) of 3.1415 

scalar:
0.00232384 elements per nanosecond
0.00246791 elements per nanosecond
0.00244565 elements per nanosecond
0.00244049 elements per nanosecond
0.00223346 elements per nanosecond
0.00228206 elements per nanosecond
0.00249925 elements per nanosecond
0.00248752 elements per nanosecond
0.00243588 elements per nanosecond
0.00247882 elements per nanosecond
0.00243925 elements per nanosecond
0.00225988 elements per nanosecond
0.00242437 elements per nanosecond
0.00204892 elements per nanosecond
0.00240362 elements per nanosecond
0.00247664 elements per nanosecond
0.00225022 elements per nanosecond
0.00248844 elements per nanosecond
0.00244302 elements per nanosecond
0.00232976 elements per nanosecond
1.22481e+06

parallelized newton-raphson using scalar f(x): ~2x speedup
0.00494962 elements per nanosecond
0.00501129 elements per nanosecond
0.00434865 elements per nanosecond
0.00502371 elements per nanosecond
0.00500942 elements per nanosecond
0.00500945 elements per nanosecond
0.00504377 elements per nanosecond
0.00501537 elements per nanosecond
0.00485419 elements per nanosecond
0.00448336 elements per nanosecond
0.00502426 elements per nanosecond
0.00504887 elements per nanosecond
0.0049874 elements per nanosecond
0.00439351 elements per nanosecond
0.004798 elements per nanosecond
0.00504869 elements per nanosecond
0.00499643 elements per nanosecond
0.00502452 elements per nanosecond
0.00482616 elements per nanosecond
0.00438719 elements per nanosecond
1.22481e+06

parallelized newton-raphson + parallel user-f(x)
0.0208567 elements per nanosecond
0.0223288 elements per nanosecond
0.0223335 elements per nanosecond
0.0223346 elements per nanosecond
0.0223441 elements per nanosecond
0.021363 elements per nanosecond
0.0223494 elements per nanosecond
0.0223393 elements per nanosecond
0.0223432 elements per nanosecond
0.0223512 elements per nanosecond
0.0219458 elements per nanosecond
0.0223504 elements per nanosecond
0.0223497 elements per nanosecond
0.0223492 elements per nanosecond
0.0223525 elements per nanosecond
0.0223508 elements per nanosecond
0.0215053 elements per nanosecond
0.0136961 elements per nanosecond
0.0221349 elements per nanosecond
0.0223504 elements per nanosecond
1.22481e+06

~9x speedup with FX8150's AVX

*/

// With godbolt.org's server that has AVX512 (and relevant compiler flags enabled)
// 0.00665837 elements per nanosecond ---> scalar
// 0.0121961 elements per nanosecond ----> parallel run with only scalar f(x) given for constructor
// 0.110637 elements per nanosecond ----> fully parallel (~17x speedup)


// Ryzen 9 7900 (avx512)
// 0.0089 elements per nanosecond ---> scalar
// 0.033 elements per nanosecond ----> parallel run with only scalar f(x) given for constructor
// 0.133 elements per nanosecond ----> fully parallel (~15x speedup) (6x FX8150)
