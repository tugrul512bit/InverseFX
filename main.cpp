#include <iostream>

#include"InverseFX.h"
int main()
{
	InverseFX::ScalarInverse<float,float> inv([](float inp){
		// black-box function sample
		// f(x)=x*x as if we are unable to compute sqrt(x) and we don't even know what f(x) is doing
		return inp*inp;
	});
	std::cout<<inv.computeInverseLowQuality(3.1415f,0.00001f)<<std::endl;
	return 0;
}

// outputs 1.77243
