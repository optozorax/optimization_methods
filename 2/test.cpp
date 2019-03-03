#include "methods.h"

//-----------------------------------------------------------------------------
double f1(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	const double cx = 5, cy = 3;
	return (x-cx)*(x-cx) + (y-cy)*(y-cy) + 15;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	std::cout << "Answer should be (5, 3)." << std::endl;

	Vector v(2);
	v << 0, 0;
	std::cout << v << std::endl;
	auto result = optimizeBroyden(f1, v, 1e-3);
	std::cout << "Answer: " << result.answer << std::endl;
	std::cout << "Iterations: " << result.iterations << ", fCount: " << result.fCount << std::endl;
	system("pause");
}