#include "methods.h"

//-----------------------------------------------------------------------------
Function setFunctionToCountCalls(int* where, const Function& f) {
	(*where) = 0;
	return [where, f](const Vector& v) -> double {
		(*where)++;
		return f(v);
	}
}

//-----------------------------------------------------------------------------
MethodResult optimizeBroyden(const Function& f1, const Vector& x0, double eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);

	// ...

	return result;
}

//-----------------------------------------------------------------------------
MethodResult optimizeConjugateGradient(const Function& f, const Vector& x0, double eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);

	// ...

	return result;
}