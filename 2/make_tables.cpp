#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "methods.h"
#include "funcs.h"
#include "visualize/visualize.h"

void makeSecondTable(const Optimizator& o, const ArgMinFunction& argmin, const Function& f, const Vector& x0, const double& eps, const std::string& file) {
	auto result = o(f, argmin, x0, eps);
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout << "x0 = " << x0 << ", eps=" << eps << std::endl;

	fout << "answer: " << result.answer << ", fCount: " << result.fCount << ", exit type: " << result.exit << ", iterations: " << result.iterations << std::endl;

	fout << "point\tvalue\tdir\tlambda\tgrad\thessian" << std::endl;
	for (auto& i : result.steps) {
		fout << i.point << "\t" << i.value << "\t" << i.dir << "\t" << i.lambda << "\t" << i.grad << "\t" << i.hessian << std::endl;
	}

	fout.close();

	//visualize(f, result.steps, 900, file);
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

	Vector x0(2); x0 << 0, 0;
	double eps = 1e-5;

	auto argmin = bindArgmin(optimizeGoldenRatio);

	makeSecondTable(optimizeBroyden, argmin, f1, x0, eps, "table2_f1_broyden");
	makeSecondTable(optimizeBroyden, argmin, f2, x0, eps, "table2_f2_broyden");
	makeSecondTable(optimizeBroyden, argmin, f3, x0, eps, "table2_f3_broyden");
	makeSecondTable(optimizeBroyden, argmin, f4, x0, eps, "table2_f4_broyden");
	makeSecondTable(optimizeBroyden, argmin, f5, x0, eps, "table2_f5_broyden");

	makeSecondTable(optimizeConjugateGradient, argmin, f1, x0, eps, "table2_f1_gradient");
	makeSecondTable(optimizeConjugateGradient, argmin, f2, x0, eps, "table2_f2_gradient");
	makeSecondTable(optimizeConjugateGradient, argmin, f3, x0, eps, "table2_f3_gradient");
	makeSecondTable(optimizeConjugateGradient, argmin, f4, x0, eps, "table2_f4_gradient");
	makeSecondTable(optimizeConjugateGradient, argmin, f5, x0, eps, "table2_f5_gradient");
}