#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle
{
private:
	Point a, b, c;

public:
	Triangle(Point first, Point second, Point third)
		: a(first), b(second), c(third)
	{};

	// Todo: методы треугольника
};


#endif //TRIANGLE_H
