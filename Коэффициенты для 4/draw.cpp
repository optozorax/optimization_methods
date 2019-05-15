#include <sstream>
#include <iostream>

#include <spob/spob.h>
#include <twg/image/image_drawing.h>

#include "../3/visualize.h"

using namespace twg;
using namespace std;

double calcFunction(
	const vector<int>& a, 
	const vector<int>& b, 
	const vector<int>& c, 
	double x, double y
) {
	#define sqr(a) ((a)*(a))
	double sum = 0;
	for (int i = 0; i < a.size(); ++i)
		sum += double(c[i])/(1.0 + sqr(x-a[i]) + sqr(y-b[i]));
	return sum;
}

void draw(
	int var,
	const FindBorders& brd,
	const vector<int>& a, 
	const vector<int>& b, 
	const vector<int>& c, 
	const string& filename) {
	ImageDrawing_aa img(brd.getCalculatedSize());

	cout << brd.getCalculatedSize().x << endl;

	double min1, max1 = 0;
	vector<vector<double>> values(img.width(), vector<double>(img.height(), 0));
	for (int i = 0; i < img.width(); i++) {
		cout << "\r" << 100 * double(i)/img.width() << "%      ";
		for (int j = 0; j < img.height(); j++) {
			vec2 pos(i, j);
			pos = brd.fromImg(pos);

			double value = calcFunction(a, b, c, pos.x, pos.y);
			values[i][j] = value;
			if (i == 0 && j == 0) {
				min1 = value;
				max1 = value;
			}
			if (value > max1) max1 = value;
			if (value < min1) min1 = value;
		}
	}

	for (int i = 0; i < img.width(); i++) {
		for (int j = 0; j < img.height(); j++) {
			const int count = 50;
			double value = values[i][j];
			value = (value - min1)/(max1-min1);
			value = sqrt(value);
			double pos = int(value * count) / double(count);
			Color clr = getGradient(pos, {Blue, White, Red});
			if (max1 == min1)
				img[Point_i(i, j)] = White;
			else
				img[Point_i(i, j)] = clr;
		}
	}

	auto draw_line = [&](spob::vec2 a, spob::vec2 b) {
		img.drawLine(brd.toImg(vec2(a.x, a.y)) + vec2(0.5, 0.5), brd.toImg(vec2(b.x, b.y)) + vec2(0.5, 0.5));
	};
	
	auto draw_arrow = [&draw_line](spob::vec2 a, spob::vec2 b, double angle, double size) { // Стрелка начинается в точке a и кончается в точке b, стрелка должна находиться в точке b
		draw_line(a, b);

		// Создаем систему координат, которая начинается в точке b, и её вектор i направлен к точке a
		spob::line2 line = spob::makeLine2(b, a);

		// Нормализуем эту систему координат, потому что вектор i равен длине вектора (b-a)
		line.normalize();

		// Создаем две точки краев стрелки
		spob::vec2 first(size, 0);
		spob::vec2 second(size, 0);

		// Один край вращаем по часовой стрелке, а другой против
		first = spob::rotate(first, spob::vec2(0), angle);
		second = spob::rotate(second, spob::vec2(0), -angle);

		// Преобразуем эти края из системы координат линии в абсолютные координаты и рисуем это
		spob::space2 space_of_line(line);
		draw_line(b, space_of_line.from(first));
		draw_line(b, space_of_line.from(second));
	};

	img.setPen(Pen(1, setAlpha(Black, 64)));
	int minmax = 13;
	for (int i = -minmax; i < minmax; i++) {
		if (i == 0) img.setPen(Pen(1.5, Blue)); else img.setPen(Pen(1, setAlpha(Black, 64)));
		draw_line(spob::vec2(-minmax, i), spob::vec2(minmax, i));
		if (i == 0) img.setPen(Pen(1.5, Red)); else img.setPen(Pen(1, setAlpha(Black, 64)));
		draw_line(spob::vec2(i, -minmax), spob::vec2(i, minmax));
	}

	{
		ImageDrawing_win imgw(&img);
		wstringstream sout;
		sout << L"Вариант: " << var;
		imgw.setTextStyle(TextStyle(20, L"Consolas", TEXT_NONE));
		imgw.setPen(Pen(1, Black));

		auto textsize = imgw.getTextSize(sout.str());
		Point_i start(5, 5);
		Point_i offset(3, 3);

		start -= offset;
		textsize += 2 * offset;

		Polygon_d poly;
		poly.array.push_back(start);
		poly.array.push_back(start + Point_d(textsize.x, 0));
		poly.array.push_back(start + textsize);
		poly.array.push_back(start + Point_d(0, textsize.y));
		imgw.setBrush(setAlpha(White, 192));
		imgw.drawPolygon(poly);

		imgw.drawText(Point_d(5, 5), sout.str());
		for (int i = 0; i < img.width(); i++) {
			for (int j = 0; j < img.height(); j++) {
				Color& clr = img[Point_i(i, j)];
				if (getAlpha(clr) == 0)
					clr = setAlpha(clr, 255);
			}
		}
	}

	img.setPen(Pen(3, Blue)); draw_arrow(spob::vec2(0, 0), spob::vec2(1, 0), deg2rad(30), 0.1);
	img.setPen(Pen(3, Red)); draw_arrow(spob::vec2(0, 0), spob::vec2(0, 1), deg2rad(30), 0.1);

	twg::saveToPng(&img, wstring(filename.begin(), filename.end()) + L".png");
}

int main() {
	FindBorders brd(500, 30, true);
	brd.process(vec2(-10, -10));
	brd.process(vec2(10, 10));
	brd.finish();

	ofstream fout("variants.txt");
	fout << "N\t";
	for (int j = 0; j < 6; ++j) fout << "c" << j+1 << "\t";
	for (int j = 0; j < 6; ++j) fout << "a" << j+1 << "\t";
	for (int j = 0; j < 6; ++j) fout << "b" << j+1 << "\t";
	fout << endl;
	for (int i = 0; i < 12; ++i) {
		fout << i+1 << "\t";
		vector<int> a, b, c;
		for (int j = 0; j < 6; ++j) {
			a.push_back(rand() % 20 - 10);
			b.push_back(rand() % 20 - 10);
			c.push_back(rand() % 10 + 1);
		}
		for (int j = 0; j < 6; ++j) fout << c[j] << "\t";
		for (int j = 0; j < 6; ++j) fout << a[j] << "\t";
		for (int j = 0; j < 6; ++j) fout << b[j] << "\t";
		
		draw(i+1, brd, a, b, c, to_string(i+1));
		fout << endl;
	}
	fout.close();
}