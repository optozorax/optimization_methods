/**
	Программа для нахождения максимально равномерно расположенных точек внутри квадрата. Будет использоваться для рейтрейсинга с задаваемым числом семплов. Число семплов от 1 до 512.

	Для этого использовался метод Бройдена с штрафными функциями.

	Результат: программа вообще не сходится. Если инициализировать точками на окружности, то программа сдвигает лишь несколько точек.

 */

#include <algorithm>
#include <random>
#include "2/methods.h"
#include "3/methods3.h"

//-----------------------------------------------------------------------------
double random(void) {
  static std::mt19937 generator(time(0));
  static std::uniform_real_distribution<double> distribution(0, 1);
  return distribution(generator);
}

using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef pair<double, double> Point;
typedef vector<Point> Points;

//-----------------------------------------------------------------------------
double getDistance(const Point& a, const Point& b) {
	#define sqr(a) ((a)*(a))
	return sqrt(sqr(a.first - b.first) + sqr(a.second - b.second));
	#undef sqr
}

//-----------------------------------------------------------------------------
Points makePoints(const Vector& x) {
	Points points;
	for (int i = 0; i < x.size(); i+=2) {
		points.push_back({x(i), x(i+1)});
	}
	return points;
}

//-----------------------------------------------------------------------------
Points makeNeighbors(const Points& points) {
	vector<int> x_neigh = {-1, 0, 1, 1, 1, 0, -1, -1};
	vector<int> y_neigh = {-1, -1, -1, 0, 1, 1, 1, 0};

	Points neighbors;
	neighbors.reserve(points.size() * 8);
	for (int i = 0; i < x_neigh.size(); ++i) {
		for (auto& j : points) {
			neighbors.push_back({j.first + x_neigh[i], j.second + y_neigh[i]});
		}
	}
	return neighbors;
}

//-----------------------------------------------------------------------------
double getMinDistance(const Points& points, int current) {
	Points neighbors = makeNeighbors(points);

	Point p = points[current];
	double distance = numeric_limits<double>::max();
	for (int i = 0; i < points.size(); ++i) {
		if (i != current) {
			distance = min(distance, getDistance(p, points[i]));
		}
	}
	for (auto& i : neighbors) {
		distance = min(distance, getDistance(p, i));
	}

	return distance;
}

//-----------------------------------------------------------------------------
double f(const Points& points) {
	double distance = numeric_limits<double>::max();

	for (int i = 0; i < points.size(); ++i) {
		distance = min(distance, getMinDistance(points, i));
	}

	// Чтобы центр масс был в центре пикселя
	/*Point massCenter(0, 0);
	for (auto& i : points) {
		massCenter.first += i.first;
		massCenter.second += i.second;
	}
	massCenter.first /= points.size();
	massCenter.second /= points.size();

	double massCoef = getDistance({0.5, 0.5}, massCenter);*/

	return 1-distance;//+massCoef;
}

//-----------------------------------------------------------------------------
double fVector(const Vector& x) {
	return f(makePoints(x));
}

//-----------------------------------------------------------------------------
double restriction(const Vector& x) {
	Points points;
	double distance = numeric_limits<double>::max();
	for (int i = 0; i < x.size(); i+=2) {
		points.push_back({x(i), x(i+1)});
	}

	// Чтобы не выходило за пределы пикселя
	double sum = 0;
	for (auto& i : points) {
		if (i.first > 1) sum += i.first - 1;
		if (i.first < 0) sum += -i.first;
		if (i.second > 1) sum += i.second - 1;
		if (i.second < 0) sum += -i.second;
	}

	return sum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	ofstream fout("samples.txt");
	auto argmin = bindArgmin(optimizeGoldenRatio);
	for (int i = 3; i <= 10; ++i) {
		Vector x0(i*2); 
		for (int j = 0; j < i; ++j) {
			x0(j*2) = 0.5 + 0.25 * sin(j * 2.0*3.1415 / i) + 0.05 * random();
			x0(j*2+1) = 0.5 + 0.25 * cos(j * 2.0*3.1415 / i) + 0.05 * random();
			// x0(j*2) = random();
			// x0(j*2+1) = random();
		}

		auto result = optimizeWithRestriction(optimizeBroyden, fVector, restriction, argmin, x0, 1e-3, 2);

		fout << i << endl;
		auto points = makePoints(result.answer);
		for (auto& i : points)
			fout << "(" << i.first << ", " << i.second << ")" << endl;
		auto neighbors = makeNeighbors(points);
		for (auto& i : neighbors)
			fout << "(" << i.first << ", " << i.second << ")" << endl;
		fout << endl << endl;

		cout 
			<< i 
			<< ", iters: " << result.iterations 
			<< ", fCount: " << result.fCount
			<< ", grad norm: " << grad(fVector, result.answer).norm() << endl;
	}
	fout.close();

	system("pause");
}