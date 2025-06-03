#include <iostream>
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
	drawer.draw_line(points[0], points.back(), color);
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

	int sign = 0;
	size_t n = points.size();

	for (size_t i = 0; i < n && sign == 0; ++i)
	{
		auto current_vector = get_directrice(points[i], points[(i + 1) % n]);
		auto next_vector = get_directrice(points[(i + 1) % n], points[(i + 2) % n]);
		int cross = cross_product(current_vector, next_vector);
		if (cross != 0)
		{
			sign = cross > 0 ? 1 : -1;
		}
	}

	if (sign == 0)
		return false;

	for (size_t i = 0; i < n; ++i)
	{
		auto current_vector = get_directrice(points[i], points[(i + 1) % n]);
		auto next_vector = get_directrice(points[(i + 1) % n], points[(i + 2) % n]);
		int cross = cross_product(current_vector, next_vector);

		if (cross != 0 && sign * cross < 0)
			return false;
	}

	return true;
}

QPoint ClipperPolygon::get_normal(const QPoint &point_1, const QPoint &point_2, const QPoint &point_3)
{
	auto edge = get_directrice(point_1, point_2);

	QPoint normal(-edge.y(), edge.x());

	auto to_inside = get_directrice(point_2, point_3);

	if (scalar_product(normal, to_inside) < 0)
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
	double t_bottom = 0, t_top = 1;

	for (size_t i = 0; i < points.size(); ++i)
	{
		auto normal = get_normal(points[i], points[(i + 1) % points.size()], points[(i + 2) % points.size()]);
		auto w_i = QPoint{segment.start().x() - points[i].x(), segment.start().y() - points[i].y()};

		std::cout << i + 1 << "-ая сторона:" << std::endl;
		double w_i_scalar = scalar_product(w_i, normal);
		double d_scalar = scalar_product(directrice, normal);
		if (fabs(d_scalar) < 1e-6)
		{
			if (w_i_scalar < 0)
				return {QPoint(), QPoint()};
			continue;
		}

		double t = -w_i_scalar / d_scalar;
		if (d_scalar > 0)
		{
			std::cout << "Ищем t нижнене, t нижнее = " << t << std::endl;
			if (t > 1)
				return {QPoint(), QPoint()};
			else
				t_bottom = std::max(t_bottom, t);
		}
		else
		{
			std::cout << "Ищем t верхнее, t верхнее = " << t << std::endl;
			if (t < 0)
				return {QPoint(), QPoint()};
			else
				t_top = std::min(t_top, t);
		}
		if (t_bottom > t_top)
			return {QPoint(), QPoint()};
	}

	return {QPoint{static_cast<int>(round(segment.start().x() + t_bottom * directrice.x())),
				   static_cast<int>(round(segment.start().y() + t_bottom * directrice.y()))},
			QPoint{static_cast<int>(round(segment.start().x() + t_top * directrice.x())),
				   static_cast<int>(round(segment.start().y() + t_top * directrice.y()))}};
}
