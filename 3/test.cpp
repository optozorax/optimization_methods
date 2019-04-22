#include "methods3.h"
#include "visualize.h"
#include "../2/visualize/find_borders.h"

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

//-----------------------------------------------------------------------------
void makeFirstTable(
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& restriction, 
	const Vector& x0, 
	const std::string& file
) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout 
		<< "x0 = " << x0 << ", "
		<< "startPenaltyCoef = " << 1 << ", "
		<< "penaltyExporent = " << 2 << std::endl;


	fout << "10^i\titer\tfCount\tanswer\trestriction_value\tgrad.norm()" << std::endl;
	for (int i = 3; i < 7; ++i) {
		double eps = pow(10.0, -double(i));
		auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, eps);
		fout 
			<< -i << "\t" 
			<< result.iterations << "\t" 
			<< result.fCount << "\t" 
			<< result.answer << "\t" 
			<< restriction(result.answer) << "\t"
			<< grad(f, result.answer).norm() << std::endl;
	}
	fout.close();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	auto restriction = restriction2;
	Vector x0(2);
	x0 << 0, 0;

	auto argmin = bindArgmin(optimizeGoldenRatio);
	makeFirstTable(argmin, f, restriction1, x0, "table_eps_1");
	makeFirstTable(argmin, f, restriction2, x0, "table_eps_2");

	FindBorders brd(500, 0, false);
	brd.process({-3, -3});
	brd.process({3, 3});
	brd.finish();
	visualizeStartPoint(brd, argmin, f, restriction1, 0.001, "1");
	//visualizeStartPoint(brd, argmin, f, restriction2, 0.001, "2");

	//system("pause");
}