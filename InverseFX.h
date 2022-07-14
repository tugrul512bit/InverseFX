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
#include<vector>
namespace InverseFX
{

	template<typename DataType>
	class ScalarDiscreteDerivative
	{
	public:
		ScalarDiscreteDerivative():fx(nullptr),inverseMultiplier(1),step(1)
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative
		ScalarDiscreteDerivative(std::function<DataType(DataType)> fxp, DataType stepPrm):
			fx(fxp), inverseMultiplier(DataType(1.0)/(DataType(2.0)*stepPrm)),
			step(stepPrm)
		{

		}

		inline
		const DataType computeTwoPointDerivativeAt(DataType inp) const noexcept
		{
			DataType val1 = fx(inp+step);
			DataType val2 = fx(inp-step);
			return (val1 - val2)*inverseMultiplier;
		}
		const DataType getStep() const { return step;}
	private:
		const std::function<DataType(DataType)> fx;
		const DataType inverseMultiplier;
		const DataType step;
	};

	template<typename DataType>
	class ParallelDiscreteDerivative
	{
	public:
		ParallelDiscreteDerivative():fx(nullptr),inverseMultiplier(1),step(1)
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative
		ParallelDiscreteDerivative(std::function<void(DataType*,DataType*,int)> fxp, DataType stepPrm):
			fx(fxp), inverseMultiplier(DataType(1.0)/(DataType(2.0)*stepPrm)),
			step(stepPrm)
		{

		}

		inline
		void computeTwoPointDerivativeAt(DataType * inp, DataType * out, int n) const noexcept
		{
			std::vector<DataType> val1(n);
			std::vector<DataType> val2(n);
			std::vector<DataType> out1(n);
			std::vector<DataType> out2(n);
			for(int i=0;i<n;i++)
			{
				val1[i]=inp[i]+step;
				val2[i]=inp[i]-step;
			}
			fx(val1.data(),out1.data(),n);
			fx(val2.data(),out2.data(),n);

			for(int i=0;i<n;i++)
			{
				 out[i]=(out1[i] - out2[i])*inverseMultiplier;
			}

		}
		const DataType getStep() const { return step;}
	private:
		const std::function<void(DataType*,DataType*,int)> fx;
		const DataType inverseMultiplier;
		const DataType step;
	};

	template<typename DataType>
	class ScalarInverse
	{
	public:
		ScalarInverse()
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative of f(x)
		ScalarInverse(std::function<DataType(DataType)> fxp, DataType stepPrm):fx(fxp),derivative(fxp,stepPrm)
		{

		}

		const DataType computeInverseLowQuality(const DataType inp) const noexcept
		{
			DataType initialGuessX = inp;
			DataType newX = inp;
			const DataType accuracy = derivative.getStep();
			DataType error=accuracy + DataType(1.0);

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

		void computeInverseLowQualityMultiple(
				const DataType * inp,
				DataType * const out,
				const int n,
				std::function<void(DataType*,DataType*,int)> fxPar
				) const noexcept
		{
			for(int i=0;i<n;i++)
			{
				DataType input = inp[i];
				DataType initialGuessX = input;
				DataType tmp = input;
				DataType tmp2 = input;
				DataType tmp3 = input;
				DataType newX = input;
				const DataType accuracy = derivative.getStep();
				DataType error=accuracy + DataType(1.0);

				// Newton-Raphson method
				// f_error = function - value
				// f'_error = derivative of function
				// new x = x - f_error/f'_error
				while(std::abs(error) > accuracy)
				{
					fxPar(&initialGuessX,&tmp,1);
					tmp2=initialGuessX+accuracy;
					tmp3=initialGuessX-accuracy;
					fxPar(&tmp2,&tmp2,1);
					fxPar(&tmp3,&tmp3,1);
					newX = initialGuessX -
							(tmp-input) /
							((tmp2 - tmp3)/(DataType(2.0)*accuracy));
					error = newX - initialGuessX;
					initialGuessX = newX;
				}
				out[i]=newX;
			}

		}

	private:
		const std::function<DataType(DataType)> fx;
		const ScalarDiscreteDerivative<DataType> derivative;
	};

	template<typename DataType>
	class ParallelInverse
	{
	public:
		ParallelInverse()
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative of f(x)
		// this constructor uses a scalar f(x) that is a bottleneck in SIMD parallelization
		ParallelInverse(std::function<DataType(DataType)> fxp, DataType stepPrm):
			fx(fxp),derivative(fxp,stepPrm),sInv(fxp,stepPrm)
		{

		}

		// stepPrm: size of +h and -h steps around x to compute f(x+-h) for discrete derivative of f(x)
		// fxParP: parallel version of f(x) for fully SIMD computations
		ParallelInverse(std::function<void(DataType*,DataType*,int)> fxParP, DataType stepPrm):
			fxPar(fxParP),derivativePar(fxParP,stepPrm)
		{

		}

		void computeInverseLowQuality(const DataType * const inp, DataType * const out, const int n) const noexcept
		{
			// if parallel fx is given, use it
			if(fxPar)
			{
				const DataType accuracy = derivative.getStep();

				constexpr int simd = 64;
				const int nSimd = n - (n%simd);

				alignas(64)
				DataType initialGuessX[simd];

				alignas(64)
				DataType newX[simd];

				alignas(64)
				DataType error[simd];

				alignas(64)
				DataType fxa[simd];

				alignas(64)
				DataType der[simd];

				alignas(64)
				DataType div[simd];

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
						error[i]=accuracy+DataType(1.0);
					}

					bool work = true;
					// Newton-Raphson method
					// f_error = function - value
					// f'_error = derivative of function
					// new x = x - f_error/f'_error
					while(work)
					{
						fxPar(initialGuessX,fxa,simd);


						for(int i=0;i<simd;i++)
						{
							fxa[i]=fxa[i]-inp[i+j];
						}

						derivativePar.computeTwoPointDerivativeAt(initialGuessX,der,simd);


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


				sInv.computeInverseLowQualityMultiple(inp+nSimd,out+nSimd,n-nSimd,fxPar);

			}
			else
			{
				const DataType accuracy = derivative.getStep();

				constexpr int simd = 64;
				const int nSimd = n - (n%simd);

				alignas(64)
				DataType initialGuessX[simd];

				alignas(64)
				DataType newX[simd];

				alignas(64)
				DataType error[simd];

				alignas(64)
				DataType fxa[simd];

				alignas(64)
				DataType der[simd];

				alignas(64)
				DataType div[simd];

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
						error[i]=accuracy+DataType(1.0);
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
		}

	private:
		const std::function<DataType(DataType)> fx;
		const ScalarDiscreteDerivative<DataType> derivative;
		const ScalarInverse<DataType> sInv;

		const std::function<void(DataType*,DataType*,int)> fxPar;
		const ParallelDiscreteDerivative<DataType> derivativePar;
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
