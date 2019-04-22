#pragma once

#include "../2/methods.h"

struct BarrierResult
{
	ExitType exit;  /// Причина выхода из большой функции
	double k;       /// Коэфиициент штрафа

	Vector answer;  /// Итоговый ответ
	int iterations; /// Число итераций больших задач
	int fCount;     /// Число вычислений функции при всех решениях задач
};

MethodResult optimizeHookeJeeves(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps);

Function sumWeight(Function f1, Function f2, double w1, double w2);

BarrierResult optimizeWithRestriction(
	const Optimizator& optimizer, 
	const Function& f, 
	const Function& restriction, 
	const ArgMinFunction& argmin, 
	const Vector& x0, 
	const double& eps,
	const double& penaltyExponent = 2,
	const double& startPenaltyCoef = 1
);

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
* задаваемой точности - таблица 1
	начальная точка, начальный штраф, коэффициент увеличения штрафа, 
	точность	число итераций	число вычислений функции	ответ	величина функции штрафа	норма градиента

* начальной точки, - нарисовать

*/