#include <twg/image/image_drawing.h>

#include "visualize.h"
#include "find_borders.h"

using namespace twg;

void visualize(const Function& f, const std::vector<StepInformation>& steps, const int& size, const std::string& filename) {
	FindBorders brd(size, 10, false);
	for (const auto& i : steps)
		brd.process(vec2(i.point(0), i.point(1)));
	brd.finish();

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

	img.setPen(Pen(2, setAlpha(Red, 192)));
	for (int i = 0; i < steps.size() - 1; i++) {
		vec2 a(steps[i].point(0), steps[i].point(1));
		vec2 b(steps[i+1].point(0), steps[i+1].point(1));
		img.drawLine(brd.toImg(a), brd.toImg(b));
	}

	img.setBrush(Brush(setAlpha(Red, 192)));
	for (int i = 0; i < steps.size(); i++) {
		vec2 a(steps[i].point(0), steps[i].point(1));
		img.drawPolygon(computeEllipse(Point_d(3, 3)).move(brd.toImg(a)));
	}

	saveToPng(&img, std::wstring(filename.begin(), filename.end()) + L".png");
}