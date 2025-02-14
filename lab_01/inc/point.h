#ifndef POINT_H
#define POINT_H

class Point {
private:
	double x;
	double y;

public:
	Point() : x(0), y(0) {}

	Point(double x_, double y_) : x(x_), y(y_) {}

	double get_x() const;
	double get_y() const;
	void set_x(double x_);
	void set_y(double y_);

	double distance(const Point &other) const;
};

#endif //POINT_H
