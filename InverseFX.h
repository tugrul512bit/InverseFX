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
#include<chrono>
#include<iostream>
namespace InverseFX
{

	template<typename InputType, typename OutputType>
	class ScalarDiscreteDerivative
	{
	public:
		ScalarDiscreteDerivative()
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative
		ScalarDiscreteDerivative(std::function<OutputType(InputType)> fxp, InputType stepPrm):
			fx(fxp), inverseMultiplier(InputType(1.0)/(InputType(2.0)*stepPrm)),
			step(stepPrm)
		{

		}

		inline
		const OutputType computeTwoPointDerivativeAt(InputType inp) const noexcept
		{
			OutputType val1 = fx(inp+step);
			OutputType val2 = fx(inp-step);
			return (val1 - val2)*inverseMultiplier;
		}
		const InputType getStep() const { return step;}
	private:
		const std::function<OutputType(InputType)> fx;
		const InputType inverseMultiplier;
		const InputType step;
	};

	template<typename OutputType, typename InputType>
	class ScalarInverse
	{
	public:
		ScalarInverse()
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative of f(x)
		ScalarInverse(std::function<OutputType(InputType)> fxp, InputType stepPrm):fx(fxp),derivative(fxp,stepPrm)
		{

		}

		const OutputType computeInverseLowQuality(const InputType inp) const noexcept
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

	template<typename OutputType, typename InputType>
	class ParallelInverse
	{
	public:
		ParallelInverse()
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative of f(x)
		ParallelInverse(std::function<OutputType(InputType)> fxp, InputType stepPrm):
			fx(fxp),derivative(fxp,stepPrm),sInv(fxp,stepPrm)
		{

		}

		void computeInverseLowQuality(const InputType * const inp, OutputType * const out, const int n) const noexcept
		{
			const InputType accuracy = derivative.getStep();

			constexpr int simd = 64;
			const int nSimd = n - (n%simd);

			alignas(64)
			InputType initialGuessX[simd];

			alignas(64)
			InputType newX[simd];

			alignas(64)
			InputType error[simd];

			alignas(64)
			InputType fxa[simd];

			alignas(64)
			InputType der[simd];

			alignas(64)
			InputType div[simd];

			alignas(64)
			int cond[simd];

			for(int j=0;j<nSimd;j+=simd)
			{
				for(int i=0;i<simd;i++)
				{
					initialGuessX[i]=inp[i+j];
				}

				for(int i=0;i<simd;i++)
				{
					newX[i]=inp[i+j];
				}

				for(int i=0;i<simd;i++)
				{
					error[i]=accuracy+InputType(1.0);
				}

				bool work = true;
				// Newton-Raphson method
				// f_error = function - value
				// f'_error = derivative of function
				// new x = x - f_error/f'_error
				while(work)
				{
					for(int i=0;i<simd;i++)
					{
						fxa[i]=fx(initialGuessX[i]);
					}

					for(int i=0;i<simd;i++)
					{
						fxa[i]=fxa[i]-inp[i+j];
					}

					for(int i=0;i<simd;i++)
					{
						der[i]=derivative.computeTwoPointDerivativeAt(initialGuessX[i]);
					}

					for(int i=0;i<simd;i++)
					{
						div[i]=fxa[i]/der[i];
					}

					for(int i=0;i<simd;i++)
					{
						newX[i]=initialGuessX[i]-div[i];
					}

					for(int i=0;i<simd;i++)
					{
						error[i]=newX[i]-initialGuessX[i];
					}

					for(int i=0;i<simd;i++)
					{
						initialGuessX[i]=newX[i];
					}

					for(int i=0;i<simd;i++)
					{
						cond[i]=std::abs(error[i]) > accuracy;
					}

					int mask = 0;
					for(int i=0;i<simd;i++)
					{
						mask += cond[i];
					}
					work = (mask>0);
				}

				for(int i=0;i<simd;i++)
				{
					out[i+j] = newX[i];
				}
			}

			for(int j=nSimd;j<n;j++)
			{
				out[j]=sInv.computeInverseLowQuality(inp[j]);
			}

		}

	private:
		const std::function<OutputType(InputType)> fx;
		const ScalarDiscreteDerivative<InputType,OutputType> derivative;
		const ScalarInverse<InputType,OutputType> sInv;
	};

	class Bench
	{
	public:
		Bench(size_t * targetPtr)
		{
			target=targetPtr;
			t1 =  std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now().time_since_epoch());
		}

		~Bench()
		{
			t2 =  std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now().time_since_epoch());
			if(target)
			{
				*target= t2.count() - t1.count();
			}
			else
			{
				std::cout << (t2.count() - t1.count())/1000000000.0 << " seconds" << std::endl;
			}
		}
	private:
		size_t * target;
		std::chrono::nanoseconds t1,t2;
	};
}


#endif /* INVERSEFX_H_ */
