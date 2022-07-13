#include<iostream>

#include"InverseFX.h"
int main()
{
	InverseFX::ScalarInverse<float,float> inv([](float inp){
		// black-box function sample
		// f(x)=x*x
		return inp*inp;
	},0.00001f);
	std::cout<<inv.computeInverseLowQuality(3.1415f)<<std::endl;
	return 0;
}

// outputs 1.77243 by calling:
// 	f(x): 3 times, 
//	multiplication: 1 time, 
//	division: 1 time
//	subtraction: 4 times
// per loop iteration (that is 5 times for this sample)
