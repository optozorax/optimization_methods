/**
	Программа для нахождения максимально равномерно расположенных точек внутри квадрата. Будет использоваться для рейтрейсинга с задаваемым числом семплов. Число семплов от 1 до 512.

	Для этого использовалась дифференциальная эволюция.

	Результат: результат намного лучше, чем для метода Бройдена, но равномерность настолько же плохая, как у обычного рандома. При ста точках можно наблюдать пустые места.

 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <differential_evolution.hpp>

using namespace std;
using namespace amichel::de;

//-----------------------------------------------------------------------------
double random(void) {
  static mt19937 generator(time(0));
  static uniform_real_distribution<double> distribution(0, 1);
  return distribution(generator);
}

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
Points makePoints(amichel::de::DVectorPtr args) {
	Points points;
	for (int i = 0; i < args->size(); i+=2) {
		points.push_back({(*args)[i], (*args)[i+1]});
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

struct fde {
public:
	virtual double operator()(amichel::de::DVectorPtr args) {
		return f(makePoints(args));
	}
};

class my_listener : public listener {
public:
	virtual void start() {}
	virtual void end() {}
	virtual void error() {}
	virtual void startGeneration(size_t genCount) {}
	virtual void endGeneration(size_t genCount, individual_ptr bestIndGen, individual_ptr bestInd) {
		if (genCount % 20 == 0)
			cout << "\r" << genCount << " " << bestInd->cost() << "             ";
	}
	virtual void startSelection(size_t genCount) {}
	virtual void endSelection(size_t genCount) {}
	virtual void startProcessors(size_t genCount) {}
	virtual void endProcessors(size_t genCount) {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	const int population_size = 400;

	ofstream fout("samples.txt");
	for (int i = 1; i <= 10; ++i) {
		try {
			constraints_ptr constraints(make_shared<constraints>(2*i, 0, 1));

			auto listener(make_shared<my_listener>());
			auto processor_listener(make_shared<null_processor_listener>());

			auto _processors(
			make_shared<processors<fde> >(3, fde(), processor_listener));

			auto terminationStrategy(make_shared<max_gen_termination_strategy>(2000));

			selection_strategy_ptr selectionStrategy(
			make_shared<best_parent_child_selection_strategy>());

			mutation_strategy_arguments mutation_arguments(1.5, 0.9);
			mutation_strategy_ptr mutationStrategy(make_shared<mutation_strategy_1>(2*i, mutation_arguments));

			differential_evolution<fde> de(2*i, population_size, _processors, constraints, true, terminationStrategy, selectionStrategy, mutationStrategy, listener);

			de.run();

			individual_ptr best(de.best());

			fout << i << endl;
			auto points = makePoints(de.best()->vars());
			for (auto& i : points)
				fout << "(" << i.first << ", " << i.second << ")" << endl;
			auto neighbors = makeNeighbors(points);
			for (auto& i : neighbors)
				fout << "(" << i.first << ", " << i.second << ")" << endl;
			fout << endl << endl;
		} catch (const amichel::de::exception& e) {
			cout << "an error occurred: " << e.what() << endl;
		}
		
		cout << i << endl;
	}
	fout.close();
	system("pause");
}