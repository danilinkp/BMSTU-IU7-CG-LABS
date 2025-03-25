#include <chrono>
#include "ellipsedrawer.h"
#include "circledrawer.h"

EllipseDrawer::EllipseDrawer(QPoint center, int rx, int ry)
	: center(center), rx(rx), ry(ry)
{}

void EllipseDrawer::put_symmetrical_points(QList<QPoint> &points, int x, int y)
{
	points.append(QPoint(center.x() + x, center.y() + y));
	points.append(QPoint(center.x() - x, center.y() + y));
	points.append(QPoint(center.x() + x, center.y() - y));
	points.append(QPoint(center.x() - x, center.y() - y));
}

// Возможно надо будет поправтить
QList<QPoint> EllipseDrawer::canonical()
{
	QList<QPoint> points;

	for (int x = 0; x <= rx; x++)
	{
		double y_exact = ry * sqrt(1.0 - static_cast<double>(x * x) / (rx * rx));
		int y = static_cast<int>(round(y_exact));

		put_symmetrical_points(points, x, y);
	}

	for (int y = 0; y <= ry; y++)
	{
		double x_exact = rx * sqrt(1.0 - static_cast<double>(y * y) / (ry * ry));
		int x = static_cast<int>(round(x_exact));

		put_symmetrical_points(points, x, y);
	}

	return points;
}

QList<QPoint> EllipseDrawer::parametric()
{
	QList<QPoint> points;

	double step = rx > ry ? 1.0 / rx : 1.0 / ry;
	double maxAngle = M_PI / 2;

	for (double t = 0; t <= maxAngle; t += step)
	{
		int x = static_cast<int>(rx * cos(t));
		int y = static_cast<int>(ry * sin(t));

		put_symmetrical_points(points, x, y);
	}

	return points;
}

QList<QPoint> EllipseDrawer::bresenham()
{
	QList<QPoint> points;

	int x = 0;
	int y = ry;

	put_symmetrical_points(points, x, y);

	int sqr_rx = rx * rx;
	int sqr_ry = ry * ry;

	int delta = sqr_ry - sqr_rx * (2 * ry + 1);

	while (y >= 0)
	{
		if (delta < 0)
		{
			int d1 = 2 * delta + sqr_rx * (2 * y + 2);
			x++;
			if (d1 < 0)
				delta += sqr_ry * (2 * x + 1);
			else
			{
				y--;
				delta += sqr_ry * (2 * x + 1) + sqr_rx * (1 - 2 * y);
			}
		}
		else if (delta > 0)
		{
			int d2 = 2 * delta + sqr_ry * (2 - 2 * x);
			y--;
			if (d2 > 0)
				delta += sqr_rx * (1 - 2 * y);
			else
			{
				x++;
				delta += sqr_ry * (2 * x + 1) + sqr_rx * (1 - 2 * y);
			}
		}
		else
		{
			y--;
			x++;
			delta += sqr_ry * (2 * x + 1) + sqr_rx * (1 - 2 * y);
		}
		put_symmetrical_points(points, x, y);
	}

	return points;
}

QList<QPoint> EllipseDrawer::middle_point()
{
	QList<QPoint> points;

	int x = 0;
	int y = ry;
	put_symmetrical_points(points, x, y);

	int sqr_rx = rx * rx;
	int sqr_ry = ry * ry;

	int p1 = sqr_ry - static_cast<int>(round(sqr_rx * (ry - 0.25)));

	while (2 * sqr_ry * x < 2 * sqr_rx * y)
	{
		x++;
		if (p1 < 0)
			p1 += sqr_ry * (2 * x + 1);
		else
		{
			y--;
			p1 += sqr_ry * (2 * x + 1) - 2 * sqr_rx * y;
		}
		put_symmetrical_points(points, x, y);
	}

	int p2 = sqr_rx - static_cast<int>(round(sqr_ry * (rx - 0.25)));
	while (y >= 0)
	{
		y--;
		if (p2 < 0)
			p2 += sqr_rx * (2 * y + 1);
		else
		{
			x++;
			p2 += sqr_rx * (2 * y + 1) - 2 * sqr_ry * x;
		}
		put_symmetrical_points(points, x, y);
	}
	return points;
}


double EllipseDrawer::time_measurement(QList<QPoint> (EllipseDrawer::*algorithm)())
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