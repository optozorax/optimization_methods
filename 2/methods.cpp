//#include "pch.h"
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
double argmin(const OneDimensionFunction& f, double eps) {
	double a, b;
	findSegment(f, 0, a, b);
	return optimizeGoldenRatio(f, a, b, eps);
}

//-----------------------------------------------------------------------------
Vector grad(const Function& f, const Vector& x1) {
	const double eps = 1e-9;
	double fx1 = f(x1);

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
MethodResult optimizeBroyden(const Function& f1, const Vector& x0, const double& eps) {
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
MethodResult optimizeConjugateGradient(const Function& f1, const Vector& x0, const double& eps, const double& epsStep) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);
	result.iterations = 0;

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
		grad0 = grad(f, x); debug(grad0);
		s0 = -grad0;		debug(s0);

		double norm = s0.norm(); debug(norm);
		if (s0.norm() < epsStep) {
			result.steps.push_back({ x1, f1(x1), s0, 0, grad0, {} });
			optimization = false;
			result.exit = ExitType::EXIT_STEP;
			break;
		}

		// processing K step ( k = 1,2,...,dim )
		for (int i = 0; i < dim; i++) {
			// calculate x(k)
			debug(s0);
			debug(x);
			auto optimizeFunc = [f, s0, x](double lambda) -> double {
				return f(x + lambda * s0);
			};
			
			double lambda = argmin(optimizeFunc, eps); debug(lambda);
			x1 = x + lambda * s0; debug(x1);
		
			//Vector sExit = lambda * s0;
			//double norm = s0.norm();				debug(norm);


			result.steps.push_back({ x1, f1(x1), s0, lambda, grad0, {} });
			result.iterations++;

			// calculate direction to x(k+1)
			grad1 = grad(f, x1);					debug(grad1);
			double w = grad1.norm() / grad0.norm(); debug(w);
			s1 = -grad1 + w * s0;					debug(s1);
		
			// prepare next iteration:
			std::swap(s0, s1);
			std::swap(grad0, grad1);
			std::swap(x, x1);

			// check exit
			//double norm = s0.norm();				debug(norm);
			if (s0.norm() < epsStep) {
				optimization = false;
				result.exit = ExitType::EXIT_STEP;
				break;
			}
		}
	}


	// k == n
	// swap x0 and x.


	//////////////////////////////////////////////
	result.answer = x;

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