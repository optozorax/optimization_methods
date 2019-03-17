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
	const std::string& file
);