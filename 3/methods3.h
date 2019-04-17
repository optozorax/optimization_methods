#pragma once

#include "../2/methods.h"

struct BarrierResult
{
	ExitType exit;
	double k;
	MethodResult result;
};

MethodResult optimizeHookeJeeves(const Function& f1, const ArgMinFunction& argmin, const Vector& x0, const double& eps);

Function sumWeight(Function f1, Function f2, double w1, double w2);

BarrierResult optimizeWithRestriction(const Optimizator& optimizer, const Function& f, const Function& restriction, const ArgMinFunction& argmin, const Vector& x0, const double& eps);