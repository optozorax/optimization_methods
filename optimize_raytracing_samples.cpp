/**
	Программа для нахождения максимально равномерно расположенных точек внутри квадрата. Будет использоваться для рейтрейсинга с задаваемым числом семплов. Число семплов от 1 до 512.

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
	return std::sqrt(sqr(a.first - b.first) + sqr(a.second - b.second));
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
	double distance = std::numeric_limits<double>::max();
	for (int i = 0; i < points.size(); ++i) {
		if (i != current) {
			distance = std::min(distance, getDistance(p, points[i]));
		}
	}
	for (auto& i : neighbors) {
		distance = std::min(distance, getDistance(p, i));
	}

	return distance;
}

//-----------------------------------------------------------------------------
double f(const Vector& x) {
	auto points = makePoints(x);
	double distance = std::numeric_limits<double>::max();

	for (int i = 0; i < points.size(); ++i) {
		distance = std::min(distance, getMinDistance(points, i));
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

	return -distance;//+massCoef;
}

//-----------------------------------------------------------------------------
double restriction(const Vector& x) {
	Points points;
	double distance = std::numeric_limits<double>::max();
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
	std::ofstream fout("samples.txt");
	auto argmin = bindArgmin(optimizeGoldenRatio);
	for (int i = 40; i <= 40; ++i) {
		BarrierResult best;
		double bestValue = 100;
		for (int j = 0; j < 3; ++j) {
			Vector x0(i*2); 
			for (int j = 0; j < i; ++j) {
				x0(j*2) = 0.5 + 0.25 * sin(j * 2.0*3.1415 / i) + 0.05 * random();
				x0(j*2+1) = 0.5 + 0.25 * cos(j * 2.0*3.1415 / i) + 0.05 * random();
				/*x0(j*2) = random();
				x0(j*2+1) = random();*/
			}

			auto result = optimizeWithRestriction(optimizeBroyden, f, restriction, argmin, x0, 1e-3, 2);

			if (f(result.answer) < bestValue) {
				best = result;
				bestValue = f(result.answer);
				cout << j << " " << bestValue << endl;
			}
		}

		fout << i << endl;
		auto points = makePoints(best.answer);
		for (auto& i : points)
			fout << "(" << i.first << ", " << i.second << ")" << endl;
		auto neighbors = makeNeighbors(points);
		for (auto& i : neighbors)
			fout << "(" << i.first << ", " << i.second << ")" << endl;
		fout << endl << endl;

		cout 
			<< i 
			<< ", iters: " << best.iterations 
			<< ", fCount: " << best.fCount
			<< ", value: " << f(best.answer) << endl;
	}
	fout.close();

	system("pause");
}