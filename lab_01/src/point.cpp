#include <cmath>
#include "point.h"

double Point::get_x() const
{
	return x;
}

double Point::get_y() const
{
	return y;
}

void Point::set_x(double x_)
{
	x = x_;
}

void Point::set_y(double y_)
{
	y = y_;
}

double Point::distance(const Point &other) const
{
	double dx = x - other.x;
	double dy = y - other.y;
	return std::sqrt(dx * dx + dy * dy);
}
