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

[[maybe_unused]] void Point::set_x(double x_)
{
	x = x_;
}

[[maybe_unused]] void Point::set_y(double y_)
{
	y = y_;
}

double Point::distance(const Point &other) const
{
	double dx = x - other.get_x();
	double dy = y - other.get_y();
	return std::sqrt(dx * dx + dy * dy);
}
