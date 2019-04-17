#include "methods3.h"
#include "../2/visualize/visualize.h"

//-----------------------------------------------------------------------------
double restriction1(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	if (y - x >= -0.2)
		return 0;
	else
		return y-x+0.2;
}

//-----------------------------------------------------------------------------
double restriction2(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return std::fabs(x+y);
}

//-----------------------------------------------------------------------------
double f(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 2 * pow(x - y, 2) + 14 * pow(y - 3, 2);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	auto restriction = restriction2;
	Vector x0(2);
	x0 << 0, 0;

	auto argmin = bindArgmin(optimizeGoldenRatio);

	auto res = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, 0.001, 2);

	std::cout << "Exit type: " << res.exit << std::endl;
	std::cout << "k: " << res.k << std::endl;

	std::cout << "Answer: " << res.result.answer << std::endl;
	std::cout << "Grad in answer: " << grad(sumWeight(f, restriction, 1, res.k), res.result.answer).norm() << std::endl;
	std::cout << "Exit type in method: " << res.result.exit << std::endl;
	std::cout << "Iterations: " << res.result.iterations << ", fCount: " << res.result.fCount << std::endl;

	visualize(optimizeHookeJeeves, optimizeBroyden, argmin, sumWeight(f, restriction, 1, res.k), x0, 0.001, 900, L"Метод Хука и Дживса", L"Метод Бройдена", "3_lab");

	system("pause");
}

/*

* выбора штрафных функций,
	Перебрать несколько степеней n
		(1/2(g+|g|))^(2n)
		функция	количество итераций	количество вычислений функции
* начальной величины коэффициента штрафа,
	коэффициент увеличения штрафа = 2
	(1, 1000) начальный штраф	количество итераций	количество вычислений функции
* стратегии изменения коэффициента штрафа
	(1, 1000) коэффициент увеличения штрафа	количество итераций	количество вычислений функции
* начальной точки, - нарисовать
* задаваемой точности - таблица 1
	начальная точка, начальный штраф, коэффициент увеличения штрафа, 
	точность	число итераций	число вычислений функции	ответ

*/