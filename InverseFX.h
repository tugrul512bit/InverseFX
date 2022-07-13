/*
 * InverseFX.h
 *
 *  Created on: Jul 13, 2022
 *      Author: tugrul
 */

#ifndef INVERSEFX_H_
#define INVERSEFX_H_
#include <cmath>
#include<functional>
namespace InverseFX
{

	template<typename InputType, typename OutputType>
	class ScalarDiscreteDerivative
	{
	public:
		ScalarDiscreteDerivative()
		{

		}
		ScalarDiscreteDerivative(std::function<OutputType(InputType)> fxp):
			fx(fxp)
		{

		}

		const OutputType computeTwoPointDerivativeAt(InputType inp, InputType step) const
		{
			OutputType val1 = fx(inp+step);
			OutputType val2 = fx(inp-step);
			return (val1 - val2)/(InputType(2.0)*step);
		}
	private:
		const std::function<OutputType(InputType)> fx;
	};

	template<typename OutputType, typename InputType>
	class ScalarInverse
	{
	public:
		ScalarInverse()
		{

		}

		ScalarInverse(std::function<OutputType(InputType)> fxp):fx(fxp),derivative(fxp)
		{

		}

		const OutputType computeInverseLowQuality(InputType inp, InputType step) const
		{
			InputType initialGuessX = inp;
			InputType newX = inp;
			InputType accuracy = step;
			InputType error=step + InputType(1.0);
			// Newton-Raphson method
			// f_error = function - value
			// f'_error = derivative of function
			// new x = x - f_error/f'_error
			while(std::abs(error) > accuracy)
			{
				newX = initialGuessX -
						(fx(initialGuessX)-inp) /
						derivative.computeTwoPointDerivativeAt(initialGuessX,step);
				error = newX - initialGuessX;
				initialGuessX = newX;
			}
			return newX;
		}
	private:
		const std::function<OutputType(InputType)> fx;
		const ScalarDiscreteDerivative<InputType,OutputType> derivative;
	};
}


#endif /* INVERSEFX_H_ */
