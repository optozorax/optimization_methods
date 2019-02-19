#include "pch.h"
#include <functional>
#include <iostream>
#include <fstream>
#include <cmath>

struct MethodResult {
	int iter;
	double answer;
	int fCount;
};

MethodResult calcDichotomy(const std::function<double(double)>& f, double a, double b, double eps) {
	int i = 0;
	int fCount = 0;
	double beta = eps*0.9;
	while (fabs(a-b) > eps) {
		double x1 = (a + b - beta) / 2.0;
		double x2 = (a + b + beta) / 2.0;
		if (f(x1) > f(x2)) {
			a = x1;
		} else {
			b = x2;
		}
		i++;
		fCount += 2;
	}
	return { i, (a + b) / 2.0, fCount };
}


MethodResult calcGoldenRatio(const std::function<double(double)>& f, double a, double b, double eps) {
	int i = 0;
	int fCount = 0;
	double beta = eps * 0.9;

	const double GOLD_A((3 - sqrt(5.0)) / 2);
	const double GOLD_B((sqrt(5.0) - 1) / 2);

	double x1 = a + GOLD_A * (b - a);
	double x2 = a + GOLD_B * (b - a);

	double f1 = f(x1);
	double f2 = f(x2);
	fCount += 2;

	while (fabs(a - b) > eps) {
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
		fCount++;
		i++;
	}
	return { i, (a + b) / 2.0, fCount };
}



MethodResult calcFibonacci(const std::function<double(double)>& f, double a, double b, double eps) {
	//auto fibonacciN = [](const int n) {return (pow(1.61803398874989, n) + pow(-0.6180339887498948, n)) / 2.23606797749978; };
	auto fibonacciN = [](const int n) {return (pow((1 + sqrt(5))/2.0, n) + pow((1 - sqrt(5)) / 2.0, n)) / sqrt(5); };


	int i = 0;
	int fCount = 0;

	int n = 0;
	for (; fibonacciN(n + 2) < (b - a) / eps; n++);

	const double fibN2 = fibonacciN(n + 2);

	const double length = b - a;
	double x1 = a + fibonacciN(n) / fibN2 * length;
	double x2 = a + b - x1;

	double f1 = f(x1);
	double f2 = f(x2);
	fCount += 2;

	for(i = 2; i < n; i++) {

		double temp = a;

		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;

			x2 = temp + (fibonacciN(n - i + 2) / fibN2) * length; 
			f2 = f(x2);

		}
		else {
			b = x2;
			x2 = x1;
			f2 = f1;

		
			x1 = temp + (fibonacciN(n - i + 1) / fibN2) * length; //
			f1 = f(x1);
		}

		fCount++;
	}

	return { i, (a + b) / 2.0, fCount };
}

void findSegment(const std::function<double(double)>& f, double x0, double& a, double& b) {
	std::ofstream fout("find_segment.txt");

	const double eps = 0.00000000100;

	double h;
	double f1 = f(x0);
	double f2 = f(x0 + eps);

	if (f1 > f2)
		h = eps;
	else
		h = -eps;


	double x1 = x0, x2 = x0, xPrevious = x0;


	int i = 0;

	do {
		xPrevious = x1;
		f1 = f2;
		x1 = x2;
		h *= 2;
		x2 = x1 + h;
		f2 = f(x2);

		fout << ++i << "\t" << x2 << "\t" << f2 << "\t[" << x1 << ", " << x2 << "]" << std::endl;
	} while (f2 < f1);
	//while ((f2 < f(x2 + 1e-9) && h < 0) || (f2 > f(x2 + 1e-9) && h > 0));

	if(h > 0) {
		a = xPrevious;
		b = x2;
	}
	else {
		a = x2;
		b = xPrevious;
	}

	fout.close();
}

int main() {
	using namespace std;
	double a = 2, b = 200, eps = 0.0001;
	auto f = [](double x) -> double { return pow(x - 15, 2.0) + 5; };
	auto min1 = calcDichotomy(f, a, b, eps);
	auto min2 = calcGoldenRatio(f, a, b, eps);
	auto min3 = calcFibonacci(f, a, b, eps);
	cout << min1.iter << " " << min1.answer << " " << min1.fCount << endl;
	cout << min2.iter << " " << min2.answer << " " << min2.fCount << endl;
	cout << min3.iter << " " << min3.answer << " " << min3.fCount << endl;

	a = 0; b = 0;
	findSegment(f, 15.000001, a, b);
	cout << a << " " << b;

	system("pause");
}