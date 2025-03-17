#include "linedrawer.h"
#include <cmath>

LineDrawer::LineDrawer(QPoint first, QPoint second)
{
	start = first;
	end = second;
}

QList<Pixel> LineDrawer::dda()
{
	QList<Pixel> points;
	if (start.x() == end.x() && start.y() == end.y())
		points.push_back(Pixel(start));
	else
	{
		double len;
		double dx = end.x() - start.x();
		double dy = end.y() - start.y();

		if (abs(dx) > abs(dy))
			len = abs(dx);
		else
			len = abs(dy);

		dx /= len;
		dy /= len;

		double x = start.x();
		double y = start.y();
		for (int i = 0; i < len; ++i)
		{
			points.push_back(Pixel(QPoint(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)))));
			x += dx;
			y += dy;
		}
	}

	return points;
}

QList<Pixel> LineDrawer::bresenham_int()
{
	QList<Pixel> points;
	if (start.x() == end.x() && start.y() == end.y())
		points.push_back(Pixel(start));
	else
	{
		int dx = end.x() - start.x();
		int dy = end.y() - start.y();

		int sign_x = sign(dx);
		int sign_y = sign(dy);

		dx = abs(dx);
		dy = abs(dy);

		bool swapped = false;

		if (dy > dx)
		{
			std::swap(dx, dy);
			swapped = true;
		}

		int error = 2 * dy - dx;

		int x = start.x();
		int y = start.y();

		while (x != end.x() || y != end.y())
		{
			points.push_back(Pixel(QPoint(x, y)));
			if (error >= 0)
			{
				swapped ? x += sign_x : y += sign_y;
				error -= 2 * dx;
			}
			if (error <= 0)
			{
				swapped ? y += sign_y : x += sign_x;
				error += 2 * dy;
			}
		}
	}

	return points;
}

QList<Pixel> LineDrawer::bresenham_float()
{
	QList<Pixel> points;
	if (start.x() == end.x() && start.y() == end.y())
		points.push_back(Pixel(start));
	else
	{
		double dx = end.x() - start.x();
		double dy = end.y() - start.y();

		int sign_x = sign(dx);
		int sign_y = sign(dy);

		dx = abs(dx);
		dy = abs(dy);

		bool swapped = false;
		if (dy > dx)
		{
			std::swap(dx, dy);
			swapped = true;
		}

		double tan = dy / dx;
		double error = tan - 0.5;

		int x = start.x();
		int y = start.y();

		while (x != end.x() || y != end.y())
		{
			QPoint new_point(x, y);
			points.push_back(Pixel(new_point));
			if (error >= 0)
			{
				swapped ? x += sign_x : y += sign_y;
				error--;
			}
			if (error <= 0)
			{
				swapped ? y += sign_y : x += sign_x;
				error += tan;
			}
		}
	}

	return points;
}

QList<Pixel> LineDrawer::bresenham_smooth()
{
	return bresenham_smooth(255);
}

QList<Pixel> LineDrawer::bresenham_smooth(int max_intensity)
{
	QList<Pixel> points;
	if (start.x() == end.x() && start.y() == end.y())
		points.push_back(Pixel(start));
	else
	{
		double dx = end.x() - start.x();
		double dy = end.y() - start.y();

		int sign_x = sign(dx);
		int sign_y = sign(dy);

		dx = abs(dx);
		dy = abs(dy);

		bool swapped = false;
		if (dy > dx)
		{
			std::swap(dx, dy);
			swapped = true;
		}

		double m = max_intensity * dy / dx;
		double error = max_intensity / 2.0;
		double w = max_intensity - m;

		int x = start.x();
		int y = start.y();

		while (x != end.x() || y != end.y())
		{
			QPoint new_point(x, y);
			points.push_back(Pixel(new_point, static_cast<int>(round(error))));
			if (error < w)
			{
				swapped ? y += sign_y : x += sign_x;
				error += m;
			}
			else
			{
				x += sign_x;
				y += sign_y;
				error -= w;
			}
		}
	}

	return points;
}

QList<Pixel> LineDrawer::wu()
{
	return wu(255);
}

static double fpart(double x)
{
	return x - std::floor(x);
}

static double rfpart(double x)
{
	return 1.0 - fpart(x);
}

QList<Pixel> LineDrawer::wu(int max_intensity)
{
	QList<Pixel> points;

	if (start.x() == end.x() && start.y() == end.y())
	{
		points.push_back(Pixel(start, max_intensity));
		return points;
	}

	bool steep = std::abs(end.y() - start.y()) > std::abs(end.x() - start.x());

	double x0 = start.x();
	double y0 = start.y();
	double x1 = end.x();
	double y1 = end.y();

	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	double dx = x1 - x0;
	double dy = y1 - y0;
	double gradient = (dx < 1e-10) ? 1.0 : dy / dx;

	double xend = std::round(x0);
	double yend = y0 + gradient * (xend - x0);
	double xgap = rfpart(x0 + 0.5);
	int xpxl1 = static_cast<int>(xend);
	int ypxl1 = static_cast<int>(std::floor(yend));

	if (steep)
	{
		points.push_back(Pixel(QPoint(ypxl1, xpxl1), static_cast<int>(rfpart(yend) * xgap * max_intensity)));
		points.push_back(Pixel(QPoint(ypxl1 + 1, xpxl1), static_cast<int>(fpart(yend) * xgap * max_intensity)));
	}
	else
	{
		points.push_back(Pixel(QPoint(xpxl1, ypxl1), static_cast<int>(rfpart(yend) * xgap * max_intensity)));
		points.push_back(Pixel(QPoint(xpxl1, ypxl1 + 1), static_cast<int>(fpart(yend) * xgap * max_intensity)));
	}

	// Основной цикл
	double intery = yend + gradient;
	xend = std::round(x1);
	int xpxl2 = static_cast<int>(xend);

	if (steep)
	{
		for (int x = xpxl1 + 1; x < xpxl2; x++)
		{
			int ypxl = static_cast<int>(std::floor(intery));
			points.push_back(Pixel(QPoint(ypxl, x), static_cast<int>(rfpart(intery) * max_intensity)));
			points.push_back(Pixel(QPoint(ypxl + 1, x), static_cast<int>(fpart(intery) * max_intensity)));
			intery += gradient;
		}
	}
	else
	{
		for (int x = xpxl1 + 1; x < xpxl2; x++)
		{
			int ypxl = static_cast<int>(std::floor(intery));
			points.push_back(Pixel(QPoint(x, ypxl), static_cast<int>(rfpart(intery) * max_intensity)));
			points.push_back(Pixel(QPoint(x, ypxl + 1), static_cast<int>(fpart(intery) * max_intensity)));
			intery += gradient;
		}
	}

	// Обработка последней точки
	yend = y1 + gradient * (xend - x1);
	xgap = fpart(x1 + 0.5);
	int ypxl2 = static_cast<int>(std::floor(yend));

	if (steep)
	{
		points.push_back(Pixel(QPoint(ypxl2, xpxl2), static_cast<int>(rfpart(yend) * xgap * max_intensity)));
		points.push_back(Pixel(QPoint(ypxl2 + 1, xpxl2), static_cast<int>(fpart(yend) * xgap * max_intensity)));
	}
	else
	{
		points.push_back(Pixel(QPoint(xpxl2, ypxl2), static_cast<int>(rfpart(yend) * xgap * max_intensity)));
		points.push_back(Pixel(QPoint(xpxl2, ypxl2 + 1), static_cast<int>(fpart(yend) * xgap * max_intensity)));
	}

	return points;
}

int LineDrawer::sign(double value)
{
	if (value < 0)
		return -1;
	else if (value > 0)
		return 1;
	return 0;
}

static long delta_time(struct timespec mt1, struct timespec mt2)
{
	return 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
}

double LineDrawer::time_measurement(QList<Pixel> (LineDrawer::*algorithm)())
{
	int ITER_COUNT_TIME = 150;
	long time1;
	double sum = 0;
	struct timespec tbegin, tend;

	for (size_t i = 0; i < ITER_COUNT_TIME; i++)
	{
		clock_gettime(CLOCK_REALTIME, &tbegin);
		(this->*algorithm)();
		clock_gettime(CLOCK_REALTIME, &tend);
		sum += delta_time(tbegin, tend);
	}

	return sum / ITER_COUNT_TIME;

}