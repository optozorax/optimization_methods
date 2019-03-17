#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "methods.h"
#include "funcs.h"
#include "visualize/visualize.h"

void makeFirstTable(const Optimizator& o, const ArgMinFunction& argmin, const Function& f, const Vector& x0, const std::string& file) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);
	fout << "x0 = " << x0 << std::endl;
	fout << "10^i\titer\tfCount\tgrad.norm()\tvalue" << std::endl;
	for (int i = 3; i < 7; ++i) {
		double eps = pow(10.0, -double(i));
		auto result = o(f, argmin, x0, eps);
		fout << -i << "\t" << result.iterations << "\t" << result.fCount << "\t" << result.steps.back().grad.norm() << "\t" << result.steps.back().value << std::endl;
	}
	fout.close();
}

void makeSecondTable(const Optimizator& o, const ArgMinFunction& argmin, const Function& f, const Vector& x0, const double& eps, const std::string& file) {
	auto result = o(f, argmin, x0, eps);
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout << "x0 = " << x0 << ", eps=" << eps << std::endl;

	fout << "answer: " << result.answer << ", fCount: " << result.fCount << ", exit type: " << result.exit << ", iterations: " << result.iterations << std::endl;

	fout << "point\tvalue\tdir\tlambda\tdiff_point\tdiff_value\tgrad\thessian" << std::endl;
	auto lastPoint = result.steps.front().point;
	auto lastValue = result.steps.front().value;
	for (auto& i : result.steps) {
		fout << i.point << "\t" << i.value << "\t" << i.dir << "\t" << i.lambda << "\t";
		fout << (lastPoint - i.point) << "\t";
		fout << std::fabs(lastValue - i.value) << "\t";
		fout << i.grad << "\t" << i.hessian << std::endl;
	}

	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int main() {
	Vector x0(2); x0 << 0, 0;
	double eps = 1e-3;

	auto argmin = bindArgmin(optimizeGoldenRatio);
	auto argminParabola = bindArgmin(optimizeParabola);

	std::vector<std::pair<Function, std::string>> funcs = {
		{f1, "f1"}, {f2, "f2"}, {f3, "f3"}, {f4, "f4"}, {f5, "f5"}
	};

	for (auto& i : funcs) {
		// Строим рисунки зависимости числа вычислений функции от положения
		visualize(optimizeBroyden, optimizeConjugateGradient, argmin, i.first, x0, 0.001, 500, "image_" + i.second);

		// Первая и вторая таблица для метода Бройдена
		makeFirstTable(optimizeBroyden, argmin, i.first, x0, "table1_" + i.second + "_broyden");
		makeSecondTable(optimizeBroyden, argmin, i.first, x0, eps, "table2_" + i.second +"_broyden");

		// Первая и вторая таблица для метода сопряженных градиентов
		makeFirstTable(optimizeConjugateGradient, argmin, i.first, x0, "table1_" + i.second + "_gradient");
		makeSecondTable(optimizeConjugateGradient, argmin, i.first, x0, eps, "table2_" + i.second +"_gradient");
	}

	makeFirstTable(optimizeBroyden, argminParabola, funcs.back().first, x0, "table1_broyden_parabola");
	makeFirstTable(optimizeConjugateGradient, argminParabola, funcs.back().first, x0, "table1_gradient_parabola");
}