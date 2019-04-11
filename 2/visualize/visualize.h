#pragma once

#include <vector>
#include <string>

#include "../methods.h"

void visualize(
	const Optimizator& o1, 
	const Optimizator& o2,
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Vector& x0, 
	const double& eps, 
	const int& size,
	const std::wstring& o1name,
	const std::wstring& o2name,
	const std::string& file
);