#include "circledrawer.h"
#include <cmath>
#include <chrono>

CircleDrawer::CircleDrawer(QPoint center, int radius)
	: center(center), radius(radius)
{}

void CircleDrawer::put_symmetrical_points(QList<QPoint> &points, int x, int y)
{
	int cx = center.x();
	int cy = center.y();
	points.append(QPoint(cx + x, cy + y));
	points.append(QPoint(cx - x, cy + y));
	points.append(QPoint(cx + x, cy - y));
	points.append(QPoint(cx - x, cy - y));
	points.append(QPoint(cx + y, cy + x));
	points.append(QPoint(cx - y, cy + x));
	points.append(QPoint(cx + y, cy - x));
	points.append(QPoint(cx - y, cy - x));
}

QList<QPoint> CircleDrawer::canonical()
{
	QList<QPoint> points;
	int x_range = static_cast<int>(ceil(radius / sqrt(2.0)));
	for (int x = 0; x <= x_range; x++)
	{
		int y = static_cast<int>(round(sqrt(radius * radius - x * x)));
		put_symmetrical_points(points, x, y);
	}

	return points;
}

QList<QPoint> CircleDrawer::parametric()
{
	QList<QPoint> points;
	double step = 1.0 / radius;
	double max_angle = M_PI / 4;
	for (double t = 0; t < max_angle + step; t += step)
	{
		int x = static_cast<int>(round(radius * cos(t)));
		int y = static_cast<int>(round(radius * sin(t)));

		put_symmetrical_points(points, x, y);
	}
	return points;
}

QList<QPoint> CircleDrawer::bresenham()
{
	QList<QPoint> points;

	int x = 0;
	int y = radius;
	int delta = 2 * (1 - radius);

	put_symmetrical_points(points, x, y);
	while (x <= y)
	{
		int delta_1 = 2 * (delta + y) - 1;
		x++;
		if (delta_1 <= 0)
			delta += 2 * x + 1;
		else
		{
			y--;
			delta += 2 * (x - y + 1);
		}
		put_symmetrical_points(points, x, y);
	}

	return points;
}

QList<QPoint> CircleDrawer::middle_point()
{
	QList<QPoint> points;

	int x = radius;
	int y = 0;
	int parameter = 1 - radius;
	put_symmetrical_points(points, x, y);
	while (x >= y)
	{
		y++;
		if (parameter < 0)
			parameter += 2 * y + 1;
		else
		{
			x--;
			parameter += 2 * (y - x) + 1;
		}
		put_symmetrical_points(points, x, y);
	}

	return points;
}


double CircleDrawer::time_measurement(QList<QPoint> (CircleDrawer::*algorithm)())
{
	int ITER_COUNT_TIME = 50;
	double sum = 0;

	for (size_t i = 0; i < ITER_COUNT_TIME; i++)
	{
		auto start = std::chrono::high_resolution_clock::now();
		(this->*algorithm)();
		auto end = std::chrono::high_resolution_clock::now();
		sum += std::chrono::duration<double, std::nano>(end - start).count();
	}

	return sum / ITER_COUNT_TIME;
}
