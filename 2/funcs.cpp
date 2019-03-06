#include "funcs.h"

//-----------------------------------------------------------------------------
double f1(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	const double cx = 5, cy = 3;
	return (x-cx)*(x-cx) + (y-cy)*(y-cy) + 15;
}

//-----------------------------------------------------------------------------
double f2(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 100 * (y - x)*(y - x) + (1 - x)*(1 - x);
}

//-----------------------------------------------------------------------------
double f3(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 100 * (y - x * x)*(y - x * x) + (1 - x)*(1 - x);
}

//-----------------------------------------------------------------------------
double f4(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	const double A1 = 1, A2 = 3, a1 = 2, a2 = 1, b1 = 3, b2 = 1, c1 = 2, c2 = 1, d1 = 3, d2 = 2;

	#define sqr(x) ((x)*(x))
	return -(A1/(1 + sqr((x-a1)/b1) + sqr((y-c1)/d1)) + A2/(1 + sqr((x-a2)/b2) + sqr((y-c2)/d2)));
	#undef sqr
}

//-----------------------------------------------------------------------------
double f5(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	const double A1 = 2, A2 = 3, a1 = 1, a2 = 2, b1 = 2, b2 = 3, c1 = 1, c2 = 3, d1 = 1, d2 = 2;

	#define sqr(x) ((x)*(x))
	return -(A1 * std::exp(-sqr((x-a1)/b1)-sqr((y-c1)/d1)) + A2 * std::exp(-sqr((x-a2)/b2)-sqr((y-c2)/d2)));
	#undef sqr
}