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
	// prepare input
	for(int i=0;i<n;i++)
	{
		arr[i]=1.01f+std::sin(clock());
	}

	size_t t;
	for(int j=0;j<20;j++)
	{
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
		{
			InverseFX::Bench bench(&t);
			invPar.computeInverseLowQuality(inp,outp,n);
		}
		std::cout<< n/(double)t << " elements per nanosecond"<<std::endl;
	}

	acc = 0;
	for(int i=0;i<n;i++)
	{
		acc +=arr[i];
	}
	std::cout<<acc<<std::endl;
	return 0;
}

// first part outputs "1.77243" by calling these 5 times:
// 	4x calls to f(x) black-box function
//	1x multiplication
//	1x division
//	4x subtractions
// second part computes array elements one by one (slow)
// third part does same in parallel (fast)

/*
output:

1.77243
0.0278211 elements per nanosecond
0.0347396 elements per nanosecond
0.0425088 elements per nanosecond
0.0491652 elements per nanosecond
0.0571772 elements per nanosecond
0.064796 elements per nanosecond
0.0817016 elements per nanosecond
0.0894475 elements per nanosecond
0.113684 elements per nanosecond
0.113131 elements per nanosecond
131011
0.112034 elements per nanosecond
0.143435 elements per nanosecond
0.143697 elements per nanosecond
0.146588 elements per nanosecond
0.146285 elements per nanosecond
0.144265 elements per nanosecond
0.144567 elements per nanosecond
0.132684 elements per nanosecond
0.143649 elements per nanosecond
0.143501 elements per nanosecond
131011

*/
