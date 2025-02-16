#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle {
private:
	Point a, b, c;

public:
	Triangle() : a(Point()), b(Point()), c(Point()) {};
	Triangle(const Point &first, const Point &second, const Point &third) : a(first), b(second), c(third) {};

	[[nodiscard]] Point get_a() const { return a; }
	[[nodiscard]] Point get_b() const { return b; }
	[[nodiscard]] Point get_c() const { return c; }

	[[nodiscard]] double area() const;
	[[nodiscard]] bool is_triangle() const;
	static double angle_between_height_and_median(const Point &vertex, const Point &opposite1, const Point &opposite2);
	void get_points_for_min_angle(Point &vertex, Point &opposite1, Point &opposite2) const;
	[[nodiscard]] double min_angle_hm() const;
};

#endif //TRIANGLE_H
