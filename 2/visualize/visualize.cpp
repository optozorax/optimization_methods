#include <sstream>

#include <twg/image/image_drawing.h>

#include "visualize.h"
#include "find_borders.h"

using namespace twg;

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
	const std::string& file) {

	auto result1 = o1(f, argmin, x0, eps);
	auto result2 = o2(f, argmin, x0, eps);

	FindBorders brd(size, 10, false);
	for (const auto& i : result1.steps)
		brd.process(vec2(i.point(0), i.point(1)));
	for (const auto& i : result2.steps)
		brd.process(vec2(i.point(0), i.point(1)));
	brd.finish();

	// Рисуем точки и линии сходимости методов из одной и той же точки
	{
		ImageDrawing_aa img(brd.getCalculatedSize());

		double min, max;
		for (int i = 0; i < img.width(); i++) {
			for (int j = 0; j < img.height(); j++) {
				vec2 pos(i, j);
				pos = brd.fromImg(pos);
				Vector x(2);
				x << pos.x, pos.y;
				double value = f(x);
				if (i == 0 && j == 0) {
					min = value;
					max = value;
				}
				if (value > max) max = value;
				if (value < min) min = value;
			}
		}

		for (int i = 0; i < img.width(); i++) {
			for (int j = 0; j < img.height(); j++) {
				vec2 pos(i, j);
				pos = brd.fromImg(pos);
				Vector x(2);
				x << pos.x, pos.y;
				double value = f(x);
				value = (value - min)/(max-min);
				 
				const int count = 20;
				Color clr = getColorBetween(int(value * count) / double(count), White, Black);

				img[Point_i(i, j)] = clr;
			}
		}

		auto drawSteps = [&] (Color clr, const auto& steps) {
			img.setPen(Pen(2, clr));
			for (int i = 0; i < steps.size() - 1; i++) {
				vec2 a(steps[i].point(0), steps[i].point(1));
				vec2 b(steps[i+1].point(0), steps[i+1].point(1));
				img.drawLine(brd.toImg(a), brd.toImg(b));
			}

			img.setBrush(Brush(clr));
			for (int i = 0; i < steps.size(); i++) {
				vec2 a(steps[i].point(0), steps[i].point(1));
				img.drawPolygon(computeEllipse(Point_d(3, 3)).move(brd.toImg(a)));
			}
		};

		drawSteps(setAlpha(Red, 192), result1.steps);
		drawSteps(setAlpha(Blue, 192), result2.steps);

		{
			ImageDrawing_win imgw(&img);
			std::wstringstream sout;
			sout << L"Траектория сходимости методов\n" + o1name + L"\n" + o2name + L"\n";
			imgw.setTextStyle(TextStyle(20, L"Consolas", TEXT_NONE));
			auto textsize = imgw.getTextSize(sout.str());
			Point_i start(5, 5);
			Point_i offset(3, 3);
				
			start -= offset;
			textsize += 2*offset;

			Polygon_d poly;
			poly.array.push_back(start);
			poly.array.push_back(start + Point_d(textsize.x, 0));
			poly.array.push_back(start + textsize);
			poly.array.push_back(start + Point_d(0, textsize.y));
			img.setBrush(setAlpha(White, 192));
			img.drawPolygon(poly);

			sout.str(L"");
			sout << L"Траектория сходимости методов" << std::endl;
			imgw.setPen(Pen(1, Black));
			imgw.drawText(Point_d(5, 5), sout.str());

			sout.str(L"");
			sout << " \n" << o1name << std::endl;
			imgw.setPen(Pen(1, Red));
			imgw.drawText(Point_d(5, 5), sout.str());

			sout.str(L"");
			sout << " \n \n" << o2name << std::endl;
			imgw.setPen(Pen(1, Blue));
			imgw.drawText(Point_d(5, 5), sout.str());

			for (int i = 0; i < img.width(); i++) {
				for (int j = 0; j < img.height(); j++) {
					Color& clr = img[Point_i(i, j)];
					if (getAlpha(clr) == 0)
						clr = setAlpha(clr, 255);
				}
			}
		}

		twg::saveToPng(&img, std::wstring(file.begin(), file.end()) + L"_0.png");
	}

	// Рисуем количество вычислений функции для этих методов из разных точек
	{
		ImageDrawing_aa img1(brd.getCalculatedSize());
		ImageDrawing_aa img2(brd.getCalculatedSize());

		double min1, max1, average1;
		double min2, max2, average2;
		std::vector<std::vector<int>> values1(img1.width(), std::vector<int>(img1.height(), 0));
		std::vector<std::vector<int>> values2(img2.width(), std::vector<int>(img2.height(), 0));
		for (int i = 0; i < img1.width(); i++) {
			for (int j = 0; j < img1.height(); j++) {
				vec2 pos(i, j);
				pos = brd.fromImg(pos);
				Vector x(2);
				x << pos.x, pos.y;

				{
					auto result = o1(f, argmin, x, eps);
					double value = result.fCount;
					//double value = rand();
					values1[i][j] = value;
					if (i == 0 && j == 0) {
						min1 = value;
						max1 = value;
					}
					if (value > max1) max1 = value;
					if (value < min1) min1 = value;

					average1 += value;
				}

				{
					auto result = o2(f, argmin, x, eps);
					double value = result.fCount;
					//double value = rand();
					values2[i][j] = value;
					if (i == 0 && j == 0) {
						min2 = value;
						max2 = value;
					}
					if (value > max2) max2 = value;
					if (value < min2) min2 = value;

					average2 += value;
				}
			}
		}

		average1 /= img1.width() * img1.height();
		average2 /= img2.width() * img2.height();

		for (int i = 0; i < img1.width(); i++) {
			for (int j = 0; j < img1.height(); j++) {
				vec2 pos(i, j);
				pos = brd.fromImg(pos);
				Vector x(2);
				x << pos.x, pos.y;

				const int count = 20;
				{
					double value = values1[i][j];
					value = (value - min1)/(max1-min1);
					Color clr = getColorBetween(int(value * count) / double(count), White, Black);
					if (max1 == min1)
						img1[Point_i(i, j)] = White;
					else
						img1[Point_i(i, j)] = clr;
				}

				{
					double value = values2[i][j];
					value = (value - min2)/(max2-min2);
					Color clr = getColorBetween(int(value * count) / double(count), White, Black);
					if (max2 == min2)
						img2[Point_i(i, j)] = White;
					else
						img2[Point_i(i, j)] = clr;
				}
			}
		}

		{
			ImageDrawing_win img(&img1);
			std::wstringstream sout;
			sout << o1name << std::endl;
			sout << "min: " << int(min1) << std::endl;
			sout << "max: " << int(max1) << std::endl;
			sout << "avg: " << int(average1) << std::endl;
			img.setTextStyle(TextStyle(20, L"Consolas", TEXT_NONE));
			img.setPen(Pen(1, Black));

			auto textsize = img.getTextSize(sout.str());
			Point_i start(5, 5);
			Point_i offset(3, 3);

			start -= offset;
			textsize += 2 * offset;

			Polygon_d poly;
			poly.array.push_back(start);
			poly.array.push_back(start + Point_d(textsize.x, 0));
			poly.array.push_back(start + textsize);
			poly.array.push_back(start + Point_d(0, textsize.y));
			img1.setBrush(setAlpha(White, 192));
			img1.drawPolygon(poly);

			img.drawText(Point_d(5, 5), sout.str());
			for (int i = 0; i < img.width(); i++) {
				for (int j = 0; j < img.height(); j++) {
					Color& clr = img[Point_i(i, j)];
					if (getAlpha(clr) == 0)
						clr = setAlpha(clr, 255);
				}
			}
		}

		{
			ImageDrawing_win img(&img2);
			std::wstringstream sout;
			sout << o2name << std::endl;
			sout << "min: " << int(min2) << std::endl;
			sout << "max: " << int(max2) << std::endl;
			sout << "avg: " << int(average2) << std::endl;
			img.setTextStyle(TextStyle(20, L"Consolas", TEXT_NONE));
			img.setPen(Pen(1, Black));

			auto textsize = img.getTextSize(sout.str());
			Point_i start(5, 5);
			Point_i offset(3, 3);

			start -= offset;
			textsize += 2 * offset;

			Polygon_d poly;
			poly.array.push_back(start);
			poly.array.push_back(start + Point_d(textsize.x, 0));
			poly.array.push_back(start + textsize);
			poly.array.push_back(start + Point_d(0, textsize.y));
			img2.setBrush(setAlpha(White, 192));
			img2.drawPolygon(poly);

			img.drawText(Point_d(5, 5), sout.str());
			for (int i = 0; i < img.width(); i++) {
				for (int j = 0; j < img.height(); j++) {
					Color& clr = img[Point_i(i, j)];
					if (getAlpha(clr) == 0)
						clr = setAlpha(clr, 255);
				}
			}
		}

		twg::saveToPng(&img1, std::wstring(file.begin(), file.end()) + L"_1.png");
		twg::saveToPng(&img2, std::wstring(file.begin(), file.end()) + L"_2.png");
	}
}