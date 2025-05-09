#include "figure.h"

Figure::Figure()
{
	closed = false;
}

void Figure::add_point(QPoint point)
{
	points.push_back(point);
}

bool Figure::is_closed() const
{
	return closed;
}

void Figure::close()
{
	closed = true;
}

size_t Figure::get_points_count() const
{
	return points.size();
}

QPoint Figure::get_point(size_t index) const
{
	return points[index];
}
