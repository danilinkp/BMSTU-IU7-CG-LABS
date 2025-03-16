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

// Поправить дублирование
QList<Pixel> LineDrawer::wu(int max_intensity)
{
	QList<Pixel> points;

	if (start.x() == end.x() && start.y() == end.y())
	{
		points.push_back(Pixel(start));
		return points;
	}

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

	double gradient = (dx == 0) ? 1.0 : dy / dx;

	int x = start.x();
	int y = start.y();

	// Первая точка
	double yend = y + gradient * 0.5;  // Смещение на половину пикселя
	double xgap = 1.0 - fmod(start.x() + 0.5, 1.0);
	int ypxl = floor(yend);



	if (swapped)
	{
		points.push_back(Pixel(QPoint(ypxl, x), static_cast<int>(max_intensity * (1.0 - (yend - ypxl)) * xgap)));
		points.push_back(Pixel(QPoint(ypxl + 1, x), static_cast<int>((max_intensity * (yend - ypxl) * xgap))));
	}
	else
	{
		points.push_back(Pixel(QPoint(x, ypxl), static_cast<int>(max_intensity * (1.0 - (yend - ypxl)) * xgap)));
		points.push_back(Pixel(QPoint(x, ypxl + 1), static_cast<int>(max_intensity * (yend - ypxl) * xgap)));
	}

	double intery = yend;
	int x_end = end.x();
	int y_end = end.y();

	if (swapped)
	{
		while (y != y_end || x != x_end)
		{
			y += sign_y;
			intery += gradient;
			int xpxl = floor(intery);
			double fraction = intery - xpxl;

			points.push_back(Pixel(QPoint(xpxl, y), static_cast<int>(max_intensity * (1.0 - fraction))));
			points.push_back(Pixel(QPoint(xpxl + 1, y), static_cast<int>(max_intensity * fraction)));

			if (fraction >= 1.0)
			{
				x += sign_x;
				intery -= 1.0;
			}
		}
	}
	else
	{
		while (x != x_end || y != y_end)
		{
			x += sign_x;
			intery += gradient;
			ypxl = floor(intery);
			double fraction = intery - ypxl;

			points.push_back(Pixel(QPoint(x, ypxl), static_cast<int>(max_intensity * (1.0 - fraction))));
			points.push_back(Pixel(QPoint(x, ypxl + 1), static_cast<int>(max_intensity * fraction)));

			if (fraction >= 1.0)
			{
				y += sign_y;
				intery -= 1.0;
			}
		}
	}

	// Последняя точка
	yend = end.y() + gradient * 0.5;
	xgap = fmod(end.x() + 0.5, 1.0);
	ypxl = floor(yend);

	if (swapped)
	{
		points.push_back(Pixel(QPoint(ypxl, end.x()), max_intensity * (1.0 - (yend - ypxl)) * xgap));
		points.push_back(Pixel(QPoint(ypxl + 1, end.x()), max_intensity * (yend - ypxl) * xgap));
	}
	else
	{
		points.push_back(Pixel(QPoint(end.x(), ypxl), max_intensity * (1.0 - (yend - ypxl)) * xgap));
		points.push_back(Pixel(QPoint(end.x(), ypxl + 1), max_intensity * (yend - ypxl) * xgap));
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
