#pragma once

#include <vector>
#include <string>

#include "../2/visualize/find_borders.h"
#include "methods3.h"

void visualizeStartPoint(
	const FindBorders& brd,
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& restriction, 
	const double& eps, 
	const std::string& filename
);