#include "clipper_polygon.h"


ClipperPolygon::ClipperPolygon(const std::vector<QPoint> &points)
{
	this->points = points;
}

void ClipperPolygon::draw(Drawer &drawer, const QColor &color) const
{
	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		const QPoint &first_point = points[i];
		const QPoint &second_point = points[(i + 1) % points.size()];
		drawer.draw_line(first_point, second_point, color);
	}
}

QPoint ClipperPolygon::get_directrice(const QPoint &point_1, const QPoint &point_2)
{
	return {point_2.x() - point_1.x(), point_2.y() - point_1.y()};
}

int ClipperPolygon::scalar_product(const QPoint &vector_1, const QPoint &vector_2)
{
	return vector_1.x() * vector_2.x() + vector_1.y() * vector_2.y();
}

int ClipperPolygon::cross_product(const QPoint &vector_1, const QPoint &vector_2)
{
	return vector_1.x() * vector_2.y() - vector_1.y() * vector_2.x();
}

bool ClipperPolygon::is_convex()
{
	if (points.size() < 3)
		return false;

	auto current_vector = get_directrice(points[0], points[1]);
	auto next_vector = get_directrice(points[1], points[2]);

	int sign = cross_product(current_vector, next_vector) > 0 ? 1 : -1;

	for (size_t i = 0; i < points.size() - 1; ++i)
	{
		current_vector = get_directrice(points[i], points[(i + 1) % points.size()]);
		next_vector = get_directrice(points[(i + 1) % points.size()], points[(i + 2) % points.size()]);
		if (sign * cross_product(current_vector, next_vector) < 0)
			return false;
	}

	if (sign < 0)
		reverse(points.begin(), points.end());

	return true;
}

QPoint ClipperPolygon::get_normal(const QPoint &point_1, const QPoint &point_2, const QPoint &point_3)
{
	auto vector = get_directrice(point_1, point_2);
	QPoint normal{};

	if (abs(vector.y()) > 1e-6)
	{
		normal.setX(1);
		normal.setY(-vector.x() / vector.y());
	}
	else
	{
		normal.setX(0);
		normal.setY(1);
	}

	if (scalar_product(QPoint{point_3.x() - point_2.x(), point_3.y() - point_2.y()}, normal) < 0)
	{
		normal.setX(-normal.x());
		normal.setY(-normal.y());
	}

	return normal;
}

LineSegment ClipperPolygon::clip(const LineSegment &segment)
{
	if (!is_convex())
		return {QPoint(), QPoint()};
	auto directrice = get_directrice(segment.start(), segment.end());
	int t_bottom = 0, t_top = 1;

	for (size_t i = 0; i < points.size(); ++i)
	{
		auto normal = get_normal(points[i], points[(i + 1) % points.size()], points[i + 2 % points.size()]);
		auto w_i = QPoint{segment.start().x() - points[i].x(), segment.start().y() - points[i].y()};

		auto w_i_scalar = scalar_product(w_i, normal);
		auto d_scalar = scalar_product(normal, directrice);
		if (abs(d_scalar) == 0)
		{
			if (w_i_scalar == 0)
				return {QPoint(), QPoint()};
			continue;
		}

		int t = -(w_i_scalar) / d_scalar;
		if (d_scalar > 0)
		{
			if (t > 1)
				return {QPoint(), QPoint()};
			else
				t_bottom = std::max(t_bottom, t);
		}
		else
		{
			if (t < 0)
				return {QPoint(), QPoint()};
			else
				t_top = std::min(t_top, t);
		}
		if (t_bottom > t_top)
			return {QPoint(), QPoint()};
	}

	return {segment.start() + directrice * t_bottom, segment.start() + directrice * t_top};
}
