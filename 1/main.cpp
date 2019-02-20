#include <functional>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>

//-----------------------------------------------------------------------------
struct MethodResult {
	int iter;
	double answer;
	int fCount;
};

typedef std::function<double(double)> Function;
typedef std::function<MethodResult(const Function&, double, double, double)> Method;

//-----------------------------------------------------------------------------
/** Метод дихотомии для поиска минимума. */
MethodResult calcDichotomy(const Function& f, double a, double b, double eps) {
	std::ofstream fout("dichotomy_file.txt");
	fout << "i\ta_i\tb_i\tb_i-a_i\t(b_(i-1)-a_(i-1))/(b_i-a_i)\tx_1\tx_2\tf(x_1)\tf(x_2)" << std::endl;
	fout << std::setprecision(9);

	int i = 0;
	int fCount = 0;
	double beta = eps*0.9;
	double lastA = a, lastB = b;
	while (std::fabs(a-b) > eps) {
		double x1 = (a + b - beta) / 2.0;
		double x2 = (a + b + beta) / 2.0;

		fout << i << "\t" << a << "\t" << b << "\t" << b-a << "\t" << (lastB-lastA)/(b-a) << "\t" << x1 << "\t" << x2 << "\t" << f(x1) << "\t" << f(x2) << std::endl;
		lastA = a; lastB = b;

		if (f(x1) > f(x2)) {
			a = x1;
		} else {
			b = x2;
		}
		i++;
		fCount += 2;
	}

	fout.close();
	return { i, (a + b) / 2.0, fCount };
}

//-----------------------------------------------------------------------------
/** Метод золотого сечения для поиска минимума. */
MethodResult calcGoldenRatio(const Function& f, double a, double b, double eps) {
	std::ofstream fout("golden_ratio_file.txt");
	fout << "i\ta_i\tb_i\tb_i-a_i\t(b_(i-1)-a_(i-1))/(b_i-a_i)\tx_1\tx_2\tf(x_1)\tf(x_2)" << std::endl;
	fout << std::setprecision(9);

	int i = 0;
	int fCount = 0;
	double beta = eps * 0.9;

	const double GOLD_A((3 - sqrt(5.0)) / 2);
	const double GOLD_B((sqrt(5.0) - 1) / 2);

	double x1 = a + GOLD_A * (b - a);
	double x2 = a + GOLD_B * (b - a);

	double f1 = f(x1);
	double f2 = f(x2);
	fCount += 2;

	double lastA = a, lastB = b;
	while (std::fabs(a - b) > eps) {
		fout << i << "\t" << a << "\t" << b << "\t" << b-a << "\t" << (lastB-lastA)/(b-a) << "\t" << x1 << "\t" << x2 << "\t" << f1 << "\t" << f2 << std::endl;
		lastA = a; lastB = b;

		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = a + GOLD_B * (b - a);
			f2 = f(x2);
		}
		else {
			b = x2;
			x2 = x1;
			f2 = f1;
			x1 = a + GOLD_A * (b - a);
			f1 = f(x1);
		}
		fCount++;
		i++;
	}

	fCount--;

	fout.close();
	return { i, (a + b) / 2.0, fCount };
}

//-----------------------------------------------------------------------------
/** Метод Фибоначчи для поиска минимума. */
MethodResult calcFibonacci(const Function& f, double a, double b, double eps) {
	// Вычисление через формулу со степенями
	auto fibonacciN = [](const int n) {return (pow((1 + sqrt(5))/2.0, n) + pow((1 - sqrt(5)) / 2.0, n)) / sqrt(5); };

	// Вычисление через сумму целых чисел
	/*auto fibonacciN = [](const int n) -> double {
		double f0 = 1;
		double f1 = 1;
		for (int i = 2; i <= n; i++) {
			double temp = f1;
			f1 = f0 + f1;
			f0 = temp;
		}
		return f1;
	};*/

	std::ofstream fout("fibonacci_file.txt");
	fout << "i\ta_i\tb_i\tb_i-a_i\t(b_(i-1)-a_(i-1))/(b_i-a_i)\tx_1\tx_2\tf(x_1)\tf(x_2)" << std::endl;
	fout << std::setprecision(9);

	int i = 0;
	int fCount = 0;

	// Ищем количество итераций
	int n = 0;
	for (; fibonacciN(n + 2) < (b - a) / eps; n++);

	const double fibN2 = fibonacciN(n + 2);

	const double length = b - a;
	double x1 = a + fibonacciN(n) / fibN2 * length;
	double x2 = a + b - x1;

	double f1 = f(x1);
	double f2 = f(x2);
	fCount += 2;

	double lastA = a, lastB = b;
	for(i = 2; i < n; i++) {
		fout << i << "\t" << a << "\t" << b << "\t" << b-a << "\t" << (lastB-lastA)/(b-a) << "\t" << x1 << "\t" << x2 << "\t" << f1 << "\t" << f2 << std::endl;
		lastA = a; lastB = b;

		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;

			x2 = a + (fibonacciN(n - i + 2) / fibN2) * length; 
			f2 = f(x2);
		} else {
			b = x2;
			x2 = x1;
			f2 = f1;
		
			x1 = a + (fibonacciN(n - i + 1) / fibN2) * length;
			f1 = f(x1);
		}

		fCount++;
	}

	fCount--;

	fout.close();

	return { i, (a + b) / 2.0, fCount };
}

//-----------------------------------------------------------------------------
/** Функция для нахождения интервала, содержащего минимум для унимодальной функции. */
void findSegment(const Function& f, double x0, double& a, double& b, double eps = 1) {
	std::ofstream fout("find_segment.txt");
	fout << "i\tx_i\tf(x_i)\tinterval" << std::endl;

	double h;
	double f1 = f(x0);
	double f2 = f(x0 + 1e-9);

	if (f1 > f2) h = eps;
	else h = -eps;

	double x1 = x0, x2 = x0, xPrevious = x0;
	int i = 0;
	do {
		xPrevious = x1;
		f1 = f2;
		x1 = x2;
		h *= 2;
		x2 = x1 + h;
		f2 = f(x2);

		fout << ++i << "\t" << x2 << "\t" << f2 << "\t[" << x1 << ", " << x2 << "]" << std::endl;
	} while (f2 < f1);

	if(h > 0) {
		a = xPrevious;
		b = x2;
	}
	else {
		a = x2;
		b = xPrevious;
	}

	fout.close();
}

//-----------------------------------------------------------------------------
/** Функция для проверки правильности работы функции findSegment. */
void checkSegment(const Function& f, double x0, double eps) {
	double a = 0, b = 0;
	findSegment(f, x0, a, b, eps);
	if (a > 15.0 || b < 15.0) {
		std::cout << "WRONG findSegment: " << x0 << " " << eps << " [" << a << ", " << b << "]" << std::endl;
	}
}

//-----------------------------------------------------------------------------
/** Построение таблиц зависимости числа вычислений функций от степени для заданного метода. */
void calcFunctionCount(const Method& m, const Function& f, double a, double b, std::string filename) {
	std::ofstream fout(filename);

	const double count = 10.0;
	for (int i = 0; i <= 7*count; i++) {
		double power = -i/count;
		fout << std::fixed << std::setprecision(2) << power;
		auto res = m(f, a, b, std::pow(10.0, power));
		fout << "\t" << res.fCount << std::endl;
	}
	
	fout.close();
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const MethodResult& m) {
	out << "iter: " << m.iter << ", x:" << m.answer << ", fCount:" << m.fCount;
	return out;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	// Начальные присвоения
	double a = 2, b = 200, eps = 1e-7;
	auto f = [](double x) -> double { return pow(x - 15, 2.0) + 5; };

	// Выводим на экран результат работы методов, а так же в файл сгенерированные таблицы
	std::cout << "Dichotomy method. " << calcDichotomy(f, a, b, eps) << std::endl;
	std::cout << "Golden ratio method. " << calcGoldenRatio(f, a, b, eps) << std::endl;
	std::cout << "Fibonacci method. " << calcFibonacci(f, a, b, eps) << std::endl;

	// Создаем файлы, где в зависимости от логарифма eps записывается число вычислений функции
	calcFunctionCount(calcDichotomy, f, a, b, "dichotomy_count.txt");
	calcFunctionCount(calcGoldenRatio, f, a, b, "golden_ratio_count.txt");
	calcFunctionCount(calcFibonacci, f, a, b, "fibonacci_count.txt");

	// Проверяем функцию findSegment на правильную работоспособность в различных точках и с различных eps
	std::vector<double> epss = {10, 1, 0.1, 0.01, 0.00001};
	for (auto& i : epss) {
		checkSegment(f, 0, i);
		checkSegment(f, 1, i);
		checkSegment(f, 13, i);
		checkSegment(f, 14, i);
		checkSegment(f, 15, i);
		checkSegment(f, 16, i);
		checkSegment(f, 100, i);
		checkSegment(f, 200, i);
		checkSegment(f, -100, i);
		checkSegment(f, 15.05, i);
		checkSegment(f, 14.95, i);
		checkSegment(f, 15.001, i);
		checkSegment(f, 15.0001, i);
	}

	// Записываем в файл таблицу поиска интервала из точки 0 с eps = 1
	findSegment(f, 0, a, b);

	std::system("pause");
}