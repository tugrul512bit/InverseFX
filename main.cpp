
#include<iostream>
#include"InverseFX.h"
int main()
{
	// scalar test
	InverseFX::ScalarInverse<float,float> inv([](float inp){
		// black-box function sample
		// f(x)=x*x
		return inp*inp;
	},0.01f);
	std::cout<<inv.computeInverseLowQuality(3.1415f)<<std::endl;

	// scalar on array test: slow
	constexpr int n = 1024*64;
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
	InverseFX::ParallelInverse<float,float> invPar([](float inp){
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

Even without a custom parallelized f(x) function, it still gains ~100% extra performance

*/
