#include "methods3.h"
#include "visualize.h"
#include "../2/visualize/find_borders.h"

//-----------------------------------------------------------------------------
double restriction1(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	if (y - x >= 0.2)
		return 0;
	else
		return -(y-x-0.2);
}

//-----------------------------------------------------------------------------
double restriction2(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return std::fabs(x+y);
}

//-----------------------------------------------------------------------------
double g(const Vector& v) {
	const double& x = v(0);
	const double& y = v(1);
	return 100*(x-y+0.2);
}

//-----------------------------------------------------------------------------
double restriction3(const Vector& v) {
	if (g(v) <= 0)
		//return -1.0 / g(v);
		return -1e-5*log(-g(v));
	else
		return std::numeric_limits<double>::infinity();
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
	const std::string& file,
	double ex = 2
) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout 
		<< "x0 = " << x0 << ", "
		<< "startPenaltyCoef = " << 1 << ", "
		<< "penaltyExponent = " << ex << std::endl;

	fout << "10^i\titer\tfCount\tanswer\trestriction_value\tgrad.norm()" << std::endl;
	for (int i = 3; i <= 7; ++i) {
		double eps = pow(10.0, -double(i));
		auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, eps, ex);
		fout 
			<< -i << "\t" 
			<< result.iterations << "\t" 
			<< result.fCount << "\t" 
			<< result.answer << "\t" 
			<< restriction(result.answer) * result.k << "\t"
			<< grad(f, result.answer).norm() << std::endl;
	}
	fout.close();
}

//-----------------------------------------------------------------------------
void makeSecondTable(
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& restriction, 
	const Vector& x0, 
	const std::string& file,
	double eps,
	double exmul = 1
) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout 
		<< "x0 = " << x0 << ", "
		<< "startPenaltyCoef = " << 1 << ", "
		<< "eps = " << eps << std::endl;

	fout << "penaltyExponent\titer\tfCount\trestriction_value\tgrad.norm()" << std::endl;
	for (double i = 1.1; i < 20; i += 0.1) {
		auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, eps, pow(i, exmul), 1);
		fout 
			<< pow(i, exmul) << "\t"
			<< result.iterations << "\t" 
			<< result.fCount << "\t" 
			<< restriction(result.answer) * result.k << "\t"
			<< grad(f, result.answer).norm() << std::endl;	
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void makeThirdTable(
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& restriction, 
	const Vector& x0, 
	const std::string& file,
	double eps,
	double ex = 2
) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout 
		<< "x0 = " << x0 << ", "
		<< "eps = " << eps << ", "
		<< "penaltyExporent = " << ex << std::endl;

	fout << "startPenaltyCoef\titer\tfCount\trestriction_value\tgrad.norm()" << std::endl;
	for (double i = 1.1; i < 20; i += 0.1) {
		auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, eps, ex, i);
		fout 
			<< i << "\t" 
			<< result.iterations << "\t" 
			<< result.fCount << "\t" 
			<< restriction(result.answer) * result.k << "\t"
			<< grad(f, result.answer).norm() << std::endl;	
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void makeFourthTable(
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& g, 
	const Vector& x0, 
	const std::string& file,
	double eps
) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout 
		<< "x0 = " << x0 << ", "
		<< "eps = " << eps << ", "
		<< "startPenaltyCoef = " << 1 << ", "
		<< "penaltyExponent = " << 0.5 << std::endl;

	fout << "n\titer\tfCount\tanswer\trestriction_value\tgrad.norm()" << std::endl;
	for (int i = 1; i < 7; ++i) {
		auto restriction = makeRestriction(i, g);
		auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x0, eps, 0.5);
		fout 
			<< i << "\t" 
			<< result.iterations << "\t" 
			<< result.fCount << "\t" 
			<< result.answer << "\t"
			<< restriction(result.answer) * result.k << "\t"
			<< grad(f, result.answer).norm() << std::endl;	
	}

	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	Vector x0(2);
	x0 << -1, 0;

	auto argmin = bindArgmin(optimizeGoldenRatio);

	makeFirstTable(argmin, f, restriction1, x0, "table_eps_1");
	makeSecondTable(argmin, f, restriction1, x0, "table_exp_1", 1e-7);
	makeThirdTable(argmin, f, restriction1, x0, "table_start_1", 1e-7);
	
	makeFirstTable(argmin, f, restriction2, x0, "table_eps_2");
	makeSecondTable(argmin, f, restriction2, x0, "table_exp_2", 1e-7);
	makeThirdTable(argmin, f, restriction2, x0, "table_start_2", 1e-7);

	makeFirstTable(argmin, f, restriction3, x0, "table_eps_3", 0.5);
	makeSecondTable(argmin, f, restriction3, x0, "table_exp_3", 1e-7, -1);
	makeThirdTable(argmin, f, restriction3, x0, "table_start_3", 1e-7, 0.5);

	makeFourthTable(argmin, f, g, x0, "table_fine_g", 0.001);

	FindBorders brd(500, 0, false);
	brd.process({-3, -3});
	brd.process({3, 3});
	brd.finish();
	visualizeStartPoint(brd, argmin, f, restriction1, 0.001, "1");
	visualizeStartPoint(brd, argmin, f, restriction2, 0.001, "2");
}