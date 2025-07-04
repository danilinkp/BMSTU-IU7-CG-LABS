#ifndef POINT_H
#define POINT_H

class Point {
private:
	double x;
	double y;

public:
	Point() : x(0), y(0) {}

	Point(double x_, double y_) : x(x_), y(y_) {}

	[[nodiscard]] double get_x() const;
	[[nodiscard]] double get_y() const;
	[[maybe_unused]] void set_x(double x_);
	[[maybe_unused]] void set_y(double y_);
	bool operator==(const Point &other) const {
		return x == other.x && y == other.y;
	}

	[[nodiscard]] double distance(const Point &other) const;
};

#endif //POINT_H
