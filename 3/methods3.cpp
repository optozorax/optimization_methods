#include "methods3.h"

//-----------------------------------------------------------------------------
MethodResult optimizeHookeJeeves(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);
	result.iterations = 0;

	double argmineps = 1e-7;

	// prepare calculation:
	Vector x = x0;
	Vector x1 = x0;
	Vector s = x0;			// direction to 1D minimization

	double f0 = f(x0), flast = f0;		// start f value
	double f1val = 0;			// value in next finding point
	//debug(x0);

	Vector zero = Vector::Zero(x0.size());
	Matrix zeroM = Matrix::Zero(x0.size(), x0.size());
	result.steps.push_back({ x, f1(x), zero, 0, grad(f1, x), zeroM });

	// optimization:
	while (true) {
		// examining search
		bool succesfulStep = false;
		double dx = 1e-2;		// start dx value

		int localIterations = 0;
		do {
			for (int i = 0; i < x1.size(); i++) {
				double fp = 0;			// value in x + dx point
				double fm = 0;			// value in x - dx point

				double temp = x1[i];
				x1[i] += dx;
				fp = f(x1);

				if (fp > f0) {
					x1[i] = temp - dx;
					fm = f(x1);

					if (fm > f0) x1[i] = temp;	// x1[i] not changed
					else f0 = fm;
				}
				else f0 = fp;
			}

			if ((x1 - x).norm() < 1e-13) dx /= 2;
			else succesfulStep = true;

			localIterations++;
			if (localIterations > 100) break;

		} while (!succesfulStep);

		// minimization in finded direction
		//debug(x1);
		//debug(x);
		s = x1 - x;		// direction
		//debug(s);

		auto optimizeFunc = [f, s, x](double lambda) -> double {
			return f(x + lambda * s);
		};

		double lambda = argmin(optimizeFunc, argmineps);
		x1 = x + lambda * s; //debug(x1);

		f1val = f(x1);

		result.steps.push_back({ x1, f1(x1), s, lambda, grad(f1, x1), zeroM });
		result.iterations++;

		// check value exit:
		//if ( < eps) break;
			//debug(x1);
		double sub = (x - x1).norm();
		double fsub = fabs(f1val - flast);
		double gg = fsub / sub;
		if (gg < eps || result.iterations > 100) break;
		else {	// prepare next iteration:
			f0 = f1val;
			flast = f0;
			x = x1;
		}

		// check step exit:
				// there shoulb be check step exit.
	}

	result.answer = x1;
	return result;
}

//-----------------------------------------------------------------------------
Function sumWeight(Function f1, Function f2, double w1, double w2) {
	return [=] (const Vector& x) -> double {
		return w1*f1(x) + w2*f2(x);
	};
}

//-----------------------------------------------------------------------------
BarrierResult optimizeWithRestriction(
	const Optimizator& optimizer, 
	const Function& f, 
	const Function& restriction, 
	const ArgMinFunction& argmin, 
	const Vector& x0, 
	const double& eps,
	const double& penaltyExponent,
	const double& startPenaltyCoef
) {
	double k = startPenaltyCoef;
	MethodResult res;
	int fCount = 0;
	for (int i = 0; i < 60; ++i) {
		k *= penaltyExponent;
		res = optimizer(sumWeight(f, restriction, 1, k), argmin, x0, eps);
		fCount += res.fCount;
		if (restriction(res.answer) < eps)
			return {EXIT_RESIDUAL, k, res.answer, i, fCount};
	}
	return {EXIT_ITERATIONS, k, res.answer, 60, fCount};
}