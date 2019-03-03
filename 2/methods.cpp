#include "methods.h"

//-----------------------------------------------------------------------------
Function setFunctionToCountCalls(int* where, const Function& f) {
	(*where) = 0;
	return [where, f](const Vector& v) -> double {
		(*where)++;
		return f(v);
	};
}

//-----------------------------------------------------------------------------
MethodResult optimizeBroyden(const Function& f1, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);

	// ...

	return result;
}

//-----------------------------------------------------------------------------
MethodResult optimizeConjugateGradient(const Function& f1, const Vector& x0, const double& eps) {
	MethodResult result;
	auto f = setFunctionToCountCalls(&result.fCount, f1);

	// ...

	return result;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Vector& v) {
	for (int i = 0; i < v.size(); ++i) {
		out << v(i);
		if (i != v.size()-1) 
			out << " ";
	}
	return out;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Matrix& m) {
	for (int i = 0; i < m.rows(); ++i) {
		for (int j = 0; j < m.cols(); ++j) {
			out << m(i);
			if (j != m.cols()-1) 
				out << " ";
		}
		if (i != m.rows()-1) 
			out << "; ";
	}
	return out;
}