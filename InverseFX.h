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
		ScalarDiscreteDerivative(std::function<OutputType(InputType)> fxp, InputType stepPrm):
			fx(fxp), inverseMultiplier(InputType(1.0)/(InputType(2.0)*stepPrm)),
			step(stepPrm)
		{

		}

		const OutputType computeTwoPointDerivativeAt(InputType inp) const
		{
			OutputType val1 = fx(inp+step);
			OutputType val2 = fx(inp-step);
			return (val1 - val2)*inverseMultiplier;
		}
		const InputType getStep() const { return step;}
	private:
		const std::function<OutputType(InputType)> fx;
		const InputType step;
		const InputType inverseMultiplier;
	};

	template<typename OutputType, typename InputType>
	class ScalarInverse
	{
	public:
		ScalarInverse()
		{

		}

		ScalarInverse(std::function<OutputType(InputType)> fxp, InputType stepPrm):fx(fxp),derivative(fxp,stepPrm)
		{

		}

		const OutputType computeInverseLowQuality(const InputType inp) const
		{
			InputType initialGuessX = inp;
			InputType newX = inp;
			const InputType accuracy = derivative.getStep();
			InputType error=accuracy + InputType(1.0);
			// Newton-Raphson method
			// f_error = function - value
			// f'_error = derivative of function
			// new x = x - f_error/f'_error
			while(std::abs(error) > accuracy)
			{
				newX = initialGuessX -
						(fx(initialGuessX)-inp) /
						derivative.computeTwoPointDerivativeAt(initialGuessX);
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
