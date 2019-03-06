#include "visualize/visualize.h"
#include "methods.h"
#include "funcs.h"

int main() {
	auto& f = f2;

	Vector v(2);
	v << 0, 0;
	auto result = optimizeConjugateGradient(f, v, 1e-5);
	//auto result = optimizeBroyden(f, v, 1e-5);
	std::cout << "Answer: " << result.answer << std::endl;
	std::cout << "Value in answer: " << f(result.answer) << std::endl;
	std::cout << "Exit type: " << result.exit << std::endl;
	std::cout << "Iterations: " << result.iterations << ", fCount: " << result.fCount << std::endl;

	visualize(f, result.steps, 900, "parabola");

	system("pause");
}