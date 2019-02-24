#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "methods.h"

//-----------------------------------------------------------------------------
double f1(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 100*(y-x)*(y-x) + (1-x)*(1-x);
}

//-----------------------------------------------------------------------------
double f2(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 100*(y-x*x)*(y-x*x) + (1-x)*(1-x);
}

//-----------------------------------------------------------------------------
double f3(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 0; // TODO узнать какой у нас вариант
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void makeSecondTable(const Optimizator& o, const Function& f, const Vector& x0, const double& eps, const std::string& file) {
	auto result = o(f, x0, eps);
	std::fstream fout(file);
	fout << std::precision(10);

	fout << "x0 = " << x0 << ", eps=" << eps << std::endl;

	for (auto& i : result.steps) {
		fout << i.point << "\t" << i.value << "\t" << i.dir << "\t" << i.lambda << "\t" << i.grad << "\t" << i.hessian << std::endl;
	}

	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int main() {
	/* 
		Для каждой функции строится:
			Первая таблица: 
				Состоит из:
					Начальное приближение
					Задаваемая точность
					Количество итераций
					Число вычислений функции
					Найденная точность
					Значение функции в точке
				Мы должны будем вручную перебирать различные:
					Начальные значения
					Задаваемую точность от 10^-3 до 10^-7
				На основании этих исследований далее надо будет сделать выводы
			Вторая таблица:
				Состоит из:
					(x_i, y_i)
					f(x_i, y_i)
					Направление поиска
					Экстремум одномерного поиска
					|x_i-x_i-1|, |y_i-y_i-1|, |f_i-f_i-1|
					Градиент функции
					Матрица вторых производных
				Точность eps=0.001
				Начальная точка одна
	*/

	Vector x0 = {0, 0};
	double eps = 0.001;

	makeSecondTable(optimizeBroyden, f1, x0, eps, "table2_f1_broyden.txt");
	makeSecondTable(optimizeBroyden, f2, x0, eps, "table2_f2_broyden.txt");
	makeSecondTable(optimizeBroyden, f3, x0, eps, "table2_f3_broyden.txt");

	makeSecondTable(optimizeConjugateGradient, f1, x0, eps, "table2_f1_gradient.txt");
	makeSecondTable(optimizeConjugateGradient, f2, x0, eps, "table2_f2_gradient.txt");
	makeSecondTable(optimizeConjugateGradient, f3, x0, eps, "table2_f3_gradient.txt");
}