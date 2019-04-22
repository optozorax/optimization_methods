#include <sstream>
#include <iostream>

#include <twg/image/image_drawing.h>

#include "visualize.h"

using namespace twg;
using namespace std;

void visualizeStartPoint(
	const FindBorders& brd,
	const ArgMinFunction& argmin, 
	const Function& f, 
	const Function& restriction,
	const double& eps, 
	const string& filename) {
	ImageDrawing_aa img(brd.getCalculatedSize());

	double min1, max1, average1 = 0;
	vector<vector<int>> values(img.width(), vector<int>(img.height(), 0));
	for (int i = 0; i < img.width(); i++) {
		cout << "\r" << 100 * double(i)/img.width() << "%      ";
		for (int j = 0; j < img.height(); j++) {
			vec2 pos(i, j);
			pos = brd.fromImg(pos);
			Vector x(2);
			x << pos.x, pos.y;

			auto result = optimizeWithRestriction(optimizeHookeJeeves, f, restriction, argmin, x, eps);
			double value = result.fCount;
			values[i][j] = value;
			if (i == 0 && j == 0) {
				min1 = value;
				max1 = value;
			}
			if (value > max1) max1 = value;
			if (value < min1) min1 = value;

			average1 += value;
		}
	}

	average1 /= img.width() * img.height();

	for (int i = 0; i < img.width(); i++) {
		for (int j = 0; j < img.height(); j++) {
			const int count = 255;
			double value = values[i][j];
			value = (value - min1)/(max1-min1);
			Color clr = getColorBetween(int(value * count) / double(count), White, Black);
			if (max1 == min1)
				img[Point_i(i, j)] = White;
			else
				img[Point_i(i, j)] = clr;
		}
	}

	{
		ImageDrawing_win imgw(&img);
		wstringstream sout;
		sout << "min: " << int(min1) << endl;
		sout << "max: " << int(max1) << endl;
		sout << "avg: " << int(average1) << endl;
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

	twg::saveToPng(&img, wstring(filename.begin(), filename.end()) + L".png");
}