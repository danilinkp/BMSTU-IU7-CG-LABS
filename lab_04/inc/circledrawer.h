#ifndef CIRCLEDRAWER_H
#define CIRCLEDRAWER_H

#include <QPoint>
#include <QList>

class CircleDrawer
{
private:
	QPoint center;
	int radius;

public:
	CircleDrawer(QPoint center, int radius);
	QList<QPoint> canonical();
	QList<QPoint> parametric();
	QList<QPoint> bresenham();
	QList<QPoint> middle_point();
	double time_measurement(QList<QPoint> (CircleDrawer::*algorithm)());
private:
	void put_symmetrical_points(QList<QPoint> &points, int x, int y);
};

#endif //CIRCLEDRAWER_H
