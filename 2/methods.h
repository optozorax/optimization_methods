#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <Eigen/Dense>

#include <functional>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

//-----------------------------------------------------------------------------
// Функции для работы с одномерными функциями
typedef std::function<double(const double&)> OneDimensionFunction;
typedef std::function<double(const OneDimensionFunction&, double, double, double)> OneDimenshionExtremumFinder;
double optimizeDichotomy(const OneDimensionFunction& f, double a, double b, double eps);
double optimizeGoldenRatio(const OneDimensionFunction& f, double a, double b, double eps);
double optimizeFibonacci(const OneDimensionFunction& f, double a, double b, double eps);
double optimizeParabola(const OneDimensionFunction& f, double a, double b, double eps);
void findSegment(const OneDimensionFunction& f, double x0, double& a, double& b, double eps = 1);

//-----------------------------------------------------------------------------
// Работа с многомерными функциями
struct MethodResult;

// Определяем используемые типы данных
typedef Eigen::MatrixXd Matrix;
typedef Eigen::VectorXd Vector;
typedef std::function<double(const Vector&)> Function;
typedef std::function<double(const OneDimensionFunction&, const double&)> ArgMinFunction;
typedef std::function<MethodResult(const Function&, const ArgMinFunction&, const Vector&, const double&)> Optimizator;

/** Доступная информация на каждом шаге. Нужна для построения таблиц и визуализации. */
struct StepInformation
{
	Vector point;   /// (x_i, y_i) - текущая точка
	double value;   /// f(x_i, y_i) - значение функции в этой точке
	Vector dir;     /// (s_1, s_2) - направление поиска
	double lambda;  /// Экстремум одномерного поиска
	Vector grad;    /// Градиент (для обоих методов необходимо)
	Matrix hessian; /// Матрица вторых производных, только для метода Бройдена
};

/** Определяет, по какой причине был совершен выход из функции. */
enum ExitType
{
	EXIT_RESIDUAL,
	EXIT_STEP,
	EXIT_ITERATIONS,
	EXIT_ERROR
};

/** Результат работы метода. */
struct MethodResult
{
	Vector answer;  /// Точка минимума
	int iterations; /// Число итераций
	ExitType exit;  /// Причина выхода
	int fCount;     /// Число вычислений функции

	// Информация о каждом шаге процесса оптимизации, необходимо для построения таблиц и визуализации
	std::vector<StepInformation> steps;
};

ArgMinFunction bindArgmin(const OneDimenshionExtremumFinder& finder);

Vector grad(const Function& f, const Vector& x);

/** Позволяет считать число вызовов функции. */
Function setFunctionToCountCalls(int* where, const Function& f);

/** Оптимизация с помощью метода Бройдена. */
MethodResult optimizeBroyden(const Function& f, const ArgMinFunction& argmin, const Vector& x0, const double& eps);

/** Оптимизация с помощью метода сопряженных градиентов Флетчера-Ривса. */
MethodResult optimizeConjugateGradient(const Function& f, const ArgMinFunction& argmin, const Vector& x0, const double& eps);

// Функции для вывода векторов и матриц.
std::string write_for_latex_double(double v, int precision);
std::ostream& operator<<(std::ostream& out, const Vector& v);
std::ostream& operator<<(std::ostream& out, const Matrix& m);

std::ostream& operator<<(std::ostream& out, const ExitType& e);