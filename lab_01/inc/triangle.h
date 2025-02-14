#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle {
private:
	Point a, b, c;

public:
	Triangle() : a(Point()), b(Point()), c(Point()) {};
	Triangle(const Point &first, const Point &second, const Point &third) : a(first), b(second), c(third) {};

	Point get_a() const { return a; }
	Point get_b() const { return b; }
	Point get_c() const { return c; }

	double area() const;
	bool is_triangle() const;
	static double angle_between_height_and_median(const Point &vertex, const Point &opposite1, const Point &opposite2);
	double min_angle_hm() const;
};

#endif //TRIANGLE_H
