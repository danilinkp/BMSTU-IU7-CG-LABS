#include "triangle.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#define EPS 1e-9

double Triangle::area() const {
	return std::abs(
		a.get_x() * (b.get_y() - c.get_y()) +
		b.get_x() * (c.get_y() - a.get_y()) +
		c.get_x() * (a.get_y() - b.get_y()) )/ 2.0;
}

bool Triangle::is_triangle() const {
	return area() > EPS;
}

double Triangle::angle_between_height_and_median(const Point &vertex, const Point &opposite1, const Point &opposite2) {
	Point M((opposite1.get_x() + opposite2.get_x()) / 2, (opposite1.get_y() + opposite2.get_y()) / 2);

	double a = opposite2.distance(opposite1);
	double b = vertex.distance(opposite1);
	double c = vertex.distance(opposite2);

	if (std::abs(b - c) < 1e-10)
		return 0.0;

	// Находим основание высоты
	// Используем теорему о пропорциональных отрезках
	double x = (b * b - c * c) / (2 * a) + a / 2;

	// Точка основания высоты
	double t = x / a; // параметр для параметрического уравнения прямой
	Point H(opposite1.get_x() + t * (opposite2.get_x() - opposite1.get_x()),
			opposite1.get_y() + t * (opposite2.get_y() - opposite1.get_y()));

	Point median_vector(M.get_x() - vertex.get_x(), M.get_y() - vertex.get_y());
	Point height_vector(H.get_x() - vertex.get_x(), H.get_y() - vertex.get_y());

	double dot_product = median_vector.get_x() * height_vector.get_x() + median_vector.get_y() * height_vector.get_y();

	// Длины векторов
	double len_median = std::sqrt(std::pow(median_vector.get_x(), 2) + std::pow(median_vector.get_y(), 2));
	double len_height = std::sqrt(std::pow(height_vector.get_x(), 2) + std::pow(height_vector.get_y(), 2));

	if (len_median < EPS || len_height < EPS)
		return 0;

	// Угол между векторами
	double cos_angle = dot_product / (len_median * len_height);
	// Защита от численных ошибок
	if (cos_angle > 1)
		cos_angle = 1;
	if (cos_angle < -1)
		cos_angle = -1;

	return std::acos(cos_angle) * 180.0 / M_PI;
}

void Triangle::get_points_for_min_angle(Point &vertex, Point &opposite1, Point &opposite2) const {
	double angle_a = angle_between_height_and_median(a, b, c);
	double angle_b = angle_between_height_and_median(b, c, a);
	double angle_c = angle_between_height_and_median(c, a, b);

	if (angle_a <= angle_b && angle_a <= angle_c) {
		vertex = a;
		opposite1 = b;
		opposite2 = c;
	} else if (angle_b <= angle_a && angle_b <= angle_c) {
		vertex = b;
		opposite1 = c;
		opposite2 = a;
	} else {
		vertex = c;
		opposite1 = a;
		opposite2 = b;
	}
}

double Triangle::min_angle_hm() const {
	double angle_A = angle_between_height_and_median(a, b, c);
	double angle_B = angle_between_height_and_median(b, a, c);
	double angle_C = angle_between_height_and_median(c, a, b);

	std::cout << angle_A << " " << angle_B << " " << angle_C << std::endl;

	return std::min({angle_A, angle_B, angle_C});
}
