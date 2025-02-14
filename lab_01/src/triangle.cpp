#include "triangle.h"
#include "cmath"
#include "algorithm"

#define EPS 1e-9

double Triangle::area() const {
	return std::abs(
		a.get_x() * (b.get_y() - c.get_y()) +
			b.get_x() * (c.get_y() - a.get_y()) +
			c.get_x() * (a.get_y() - b.get_y())
	) / 2.0;
}

bool Triangle::is_triangle() const {
	return area() > EPS;
}

double Triangle::angle_between_height_and_median(const Point &vertex,
												 const Point &opposite1,
												 const Point &opposite2) {
	// Середина стороны, противоположной vertex
	Point M((opposite1.get_x() + opposite2.get_x()) / 2, (opposite1.get_y() + opposite2.get_y()) / 2);

	// Уравнение стороны opposite1-opposite2: (y - y2) = k * (x - x2)
	double k_side = (opposite2.get_y() - opposite1.get_y()) / (opposite2.get_x() - opposite1.get_x());

	// Уравнение высоты: (y - vertex_y) = k_height * (x - vertex_x), где k_height = -1 / k_side
	double k_height = -1 / k_side;

	// Находим точку пересечения H высоты и стороны opposite1-opposite2
	double x_H = (k_side * opposite1.get_x() - k_height * vertex.get_x() + vertex.get_y() - opposite1.get_y())
		/ (k_side - k_height);
	double y_H = k_side * (x_H - opposite1.get_x()) + opposite1.get_y();
	Point H(x_H, y_H);

	// Вектор медианы vertex-M
	Point median_vector(M.get_x() - vertex.get_x(), M.get_y() - vertex.get_y());

	// Вектор высоты vertex-H
	Point height_vector(H.get_x() - vertex.get_x(), H.get_y() - vertex.get_y());

	// Скалярное произведение векторов
	double dot_product = median_vector.get_x() * height_vector.get_x() + median_vector.get_y() * height_vector.get_y();

	// Длины векторов
	double len_median = std::hypot(median_vector.get_x(), median_vector.get_y());
	double len_height = std::hypot(height_vector.get_x(), height_vector.get_y());

	// Угол между векторами (в радианах)
	double angle = std::acos(dot_product / (len_median * len_height));

	return angle * 180 / M_PI;
}

double Triangle::min_angle_hm() const {
	double angle_A = angle_between_height_and_median(a, b, c);
	double angle_B = angle_between_height_and_median(b, a, c);
	double angle_C = angle_between_height_and_median(c, a, b);

	return std::min({angle_A, angle_B, angle_C});
}