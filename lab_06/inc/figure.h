#ifndef FIGURE_H
#define FIGURE_H


#include <QPoint>
#include <QList>

class Figure
{
public:
	Figure();

	void add_point(QPoint point);
	[[nodiscard]] bool is_closed() const;
	void close();
	[[nodiscard]] size_t get_points_count() const;
	[[nodiscard]] QPoint get_point(size_t index) const;

private:
	QList<QPoint> points;
	bool closed = false;
};


#endif //FIGURE_H
