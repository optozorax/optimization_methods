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
Function makeRestriction(int n, Function g);

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