#include "pch.h"
#include "methods.h"

//-----------------------------------------------------------------------------
double optimizeDichotomy(const OneDimensionFunction& f, double a, double b, double eps) {
	double beta = eps*0.9;
	double x1, x2;
	while (std::fabs(a-b) > eps) {
		x1 = (a + b - beta) / 2.0;
		x2 = (a + b + beta) / 2.0;

		if (f(x1) > f(x2)) {
			a = x1;
		} else {
			b = x2;
		}
	}

	return (a + b) / 2.0;
}

//-----------------------------------------------------------------------------
double optimizeGoldenRatio(const OneDimensionFunction& f, double a, double b, double eps) {
	double beta = eps * 0.9;

	const double GOLD_A((3 - sqrt(5.0)) / 2);
	const double GOLD_B((sqrt(5.0) - 1) / 2);

	double x1 = a + GOLD_A * (b - a);
	double x2 = a + GOLD_B * (b - a);

	double f1 = f(x1);
	double f2 = f(x2);

	while (std::fabs(a - b) > eps) {
		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = a + GOLD_B * (b - a);
			f2 = f(x2);
		}
		else {
			b = x2;
			x2 = x1;
			f2 = f1;
			x1 = a + GOLD_A * (b - a);
			f1 = f(x1);
		}
	}

	return (a + b) / 2.0;
}

//-----------------------------------------------------------------------------
double optimizeFibonacci(const OneDimensionFunction& f, double a, double b, double eps) {
	// Вычисление через сумму целых чисел
	auto fibonacciN = [](const int n) -> double {
		double f0 = 1;
		double f1 = 1;
		for (int i = 2; i <= n; i++) {
			double temp = f1;
			f1 = f0 + f1;
			f0 = temp;
		}
		return f1;
	};

	// Ищем количество итераций
	int n = 0;
	for (; fibonacciN(n + 2) < (b - a) / eps; n++);

	const double fibN2 = fibonacciN(n + 2);

	const double length = b - a;
	double x1 = a + fibonacciN(n) / fibN2 * length;
	double x2 = a + b - x1;

	double f1 = f(x1);
	double f2 = f(x2);

	for(int i = 2; i < n+2; i++) {
		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;

			x2 = a + (fibonacciN(n - i + 2) / fibN2) * length; 
			f2 = f(x2);
		} else {
			b = x2;
			x2 = x1;
			f2 = f1;
		
			x1 = a + (fibonacciN(n - i + 1) / fibN2) * length;
			f1 = f(x1);
		}
	}

	return (a + b) / 2.0;
}

//-----------------------------------------------------------------------------
double optimizeParabola(const OneDimensionFunction& f, double a, double b, double eps) {
	/*кроме унимодальности, налагается дополнительное требование
	достаточной гладкости (по крайней мере, непрерывности).
	// x1 < x2 < x3
	// f(x1) >= f(x2)
	// f(x3) >= f(x2)
	*/

	double x1 = a;
	double x2 = (a + b) / 2;
	double x3 = b;
	double x = 0;			// current approximate xmin
	double xPrev = 0;		// pervious approximate xmin

	double f1 = f(x1);
	double f2 = f(x2);
	double f3 = f(x3);

	std::cout << "x: " << x1 << " " << x2 << " " << x3 << std::endl;
	std::cout << "f: " << f1 << " " << f2 << " " << f3 << std::endl;

	bool finding = true;
	int iter = 0;
	while (finding) {

		// parabola y = a0 + a1(x - x1) + a2(x - x1)(x - x2);
		double a0 = f1;
		double a1 = (f2 - f1) / (x2 - x1);
		double a2 = ((f3 - f1) / (x3 - x1) - (f2 - f1) / (x2 - x1)) / (x3 - x2);

		x = 0.5 * (x1 + x2 - a1 / a2); // next approximation of min point

		// check out
		if (iter++ > 0) {
			if (fabs(x - xPrev) < eps) {
				finding = false;
				break;
			}
		}

		double fx = f(x);

		//std::cout.precision(15);
		//std::cout << "_________________________________________________________" << std::endl;
		//std::cout << iter << " " << x << " " << fx << std::endl;

		// find new section:
		xPrev = x;

		if (x <= x2) {
			if (fx >= f2) {
				// xmin  in [x, x3]
				x1 = x; f1 = fx;
				// x2 and x3 stay the same
			}
			else {	// fx < f2 
				// xmin in [x1, x2]
				// x1 stay the same
				x3 = x2; f3 = f2;
				x2 = x; f2 = fx;
			}
		}
		else { // x > x2
			if (fx > f2) {
				// xmin in [x1, x]
				// x1 and x2 stay the same
				x3 = x, f3 = fx;
			}
			else {// fx <= f2 
				// xmin in [x2, x3]
				// x3 stay the same
				x1 = x2; f1 = f2;
				x2 = x; f2 = fx;
			}
		}

		//std::cout <<"x: " << x1 << " " << x2 << " " << x3 << std::endl;
		//std::cout << "f: " << f1 << " " << f2 << " " << f3 << std::endl;

	}
	return x;
}

//-----------------------------------------------------------------------------
void findSegment(const OneDimensionFunction& f, double x0, double& a, double& b, double eps) {
	double h;
	double f1 = f(x0);
	double f2 = f(x0 + 1e-9);

	if (f1 > f2) h = eps;
	else h = -eps;

	double x1 = x0, x2 = x0, xPrevious = x0;
	int i = 0;
	do {
		xPrevious = x1;
		f1 = f2;
		x1 = x2;
		h *= 2;
		x2 = x1 + h;
		f2 = f(x2);

		if (h > 512) break; // Костыль, потому что бывают не унимодальные функции
	} while (f2 < f1);

	if(h > 0) {
		a = xPrevious;
		b = x2;
	}
	else {
		a = x2;
		b = xPrevious;
	}

	// Дополнение к костылю
	if (h > 512) {
		if (h > 0) {
			a = x0;
			b = x2;
		} else {
			a = x2;
			b = x0;
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ArgMinFunction bindArgmin(const OneDimenshionExtremumFinder& finder) {
	return [finder](const OneDimensionFunction& f, double eps) {
		double a, b;
		findSegment(f, 0, a, b);
		return finder(f, a, b, eps);
	};
}

//-----------------------------------------------------------------------------
Vector grad(const Function& f, const Vector& x1) {
	const double eps = 1e-9;
	const double fx1 = f(x1);

	Vector result(x1.size());
	Vector x = x1;

	for (int i = 0; i < x.size(); ++i) {
		x(i) += eps;
		result[i] = (f(x) - fx1) / eps;
		x(i) -= eps;
	}
	return result;
}

//-----------------------------------------------------------------------------
Function setFunctionToCountCalls(int* where, const Function& f) {
	(*where) = 0;
	return [where, f](const Vector& v) -> double {
		(*where)++;
		return f(v);
	};
}

//-----------------------------------------------------------------------------
MethodResult optimizeBroyden(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);
	result.iterations = 0;

	#ifdef _DEBUG
	#define debug(a) std::cout << #a << ": " << (a) << std::endl;
	#else
	#define debug(a) ;
	#endif

	Vector x = x0; debug(x);
	Vector lastx = x;

	Matrix n;
	n = Matrix::Identity(x.size(), x.size()); debug(n);

	result.steps.push_back({ x, f1(x), {}, 0, {}, {} });

	Vector gradf = grad(f, x);  debug(gradf);
	while (true) {
		debug(gradf.norm());
		if (gradf.norm() < eps) {
			result.exit = ExitType::EXIT_RESIDUAL;
			break;
		}

		Vector ngradf = n * gradf; debug(ngradf);
		auto optimizeFunc = [f, ngradf, x] (double lambda) -> double {
			return f(x - lambda * ngradf);
		};
		double lambda = argmin(optimizeFunc, eps); debug(lambda);

		Vector dx = -lambda * ngradf; debug(dx);
		x = x + dx; debug(x);
		Vector old_gradf = gradf;
		gradf = grad(f, x); debug(gradf);
		Vector dg = gradf - old_gradf; debug(dg);
		Vector temp = dx - n * dg; debug(temp);
		Matrix dn = temp * temp.transpose() / (temp.transpose() * dg); debug(dn);
		n = n + dn; debug(n);
		result.iterations++;

		result.steps.push_back({ x, f1(x), ngradf, lambda, gradf, n });

		if ((x - lastx).norm() < eps) {
			result.exit = ExitType::EXIT_STEP;
			break;
		}
		/*if (result.iterations > 200) {
			result.exit = ExitType::EXIT_ITERATIONS;
			break;
		}*/

		lastx = x;
	}

	result.answer = x;

	return result;
}

//-----------------------------------------------------------------------------
MethodResult optimizeConjugateGradient(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);
	result.iterations = 0;

	double argmineps = 1e-7;
	double gradNorm = 0, grad1Norm = 0;

	/////////////////////////////////////////////////////
#ifdef _DEBUG
#define debug(a) std::cout << #a << ": " << (a) << std::endl;
#else
#define debug(a) ;
#endif

	// prepare calculation:
	int dim = x0.size();

	Vector x = x0, x1(dim);
	Vector s0(dim), s1(dim);
	Vector grad0(dim), grad1(dim);

	bool optimization = true;
	while (optimization) {
		// find first direction
		grad0 = grad(f, x); //debug(grad0);
		s0 = -grad0;		//debug(s0);

		gradNorm = s0.norm();
		if (gradNorm < eps) {
			result.steps.push_back({ x1, f1(x1), s0, 0, grad0, {} });
			optimization = false;
			result.exit = ExitType::EXIT_STEP;
			break;
		}

		// processing K step ( k = 1,2,...,dim )
		for (int i = 0; i < dim+1; i++) {
			// calculate x(k)
		//	debug(s0);
		//	debug(x);
			auto optimizeFunc = [f, s0, x](double lambda) -> double {
				return f(x + lambda * s0);
			};
			
			double lambda = argmin(optimizeFunc, argmineps); //debug(lambda);
			x1 = x + lambda * s0; //debug(x1);

			result.steps.push_back({ x1, f1(x1), s0, lambda, grad0, {} });
			result.iterations++;

			// calculate direction to x(k+1)
			grad1 = grad(f, x1);					//debug(grad1);
			grad1Norm = grad1.norm();		  //debug(grad1Norm);
			double w = grad1Norm / gradNorm; //debug(w);
			s1 = -grad1 + w * s0;					//debug(s1);
		
			// prepare next iteration:
			std::swap(s0, s1);
			std::swap(grad0, grad1);
			std::swap(x, x1);
			std::swap(gradNorm, grad1Norm);

			// check exit
			//double norm = s0.norm();				debug(norm);
			if (s0.norm() < eps) {
				optimization = false;
				result.exit = ExitType::EXIT_STEP;
				break;
			}

			debug(i);
		}
	}


	// k == n
	// swap x0 and x.


	//////////////////////////////////////////////
	result.answer = x;

	return result;
}

//-----------------------------------------------------------------------------
MethodResult optimizeHookeJeeves(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);
	result.iterations = 0;

	double argmineps = 1e-7;

	// prepare calculation:
	double dx = 1e-3;		// start dx value
	Vector x = x0;
	Vector x1 = x0;
	Vector s = x0;			// direction to 1D minimization

	double f0 = f(x0);		// start f value
	double f1val = 0;			// value in next finding point

	// optimization:
	while (true) {
		// examining search
		bool succesfulStep = false;

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

			if (x1 == x) dx /= 2;
			else succesfulStep = true;

		} while (!succesfulStep);

		// minimization in finded direction 
		s = x1 - x;		// direction

		auto optimizeFunc = [f, s, x](double lambda) -> double {
			return f(x + lambda * s);
		};

		double lambda = argmin(optimizeFunc, argmineps);
		x1 = x0 + lambda * s; //debug(x1);

		f1val = f(x1);

		// check value exit:
		if (abs(f1val - f0) < eps) break;
		else {	// prepare next iteration:
			std::swap(f1val, f0);
			std::swap(x1, x);
		}

		// check step exit:
				// there shoulb be check step exit.
	}

	result.answer = x1;
	return result;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Vector& v) {
	for (int i = 0; i < v.size(); ++i) {
		out << v(i);
		if (i != v.size()-1) 
			out << " ";
	}
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Matrix& m) {
	for (int i = 0; i < m.rows(); ++i) {
		for (int j = 0; j < m.cols(); ++j) {
			out << m(i, j);
			if (j != m.cols()-1) 
				out << " ";
		}
		if (i != m.rows()-1) 
			out << "; ";
	}
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const ExitType& e) {
	switch (e) {
		case EXIT_RESIDUAL: out << "by residual"; break;
		case EXIT_STEP: out << "by step"; break;
		case EXIT_ITERATIONS: out << "by iterations"; break;
		case EXIT_ERROR: out << "by error"; break;
	}
	return out;
}