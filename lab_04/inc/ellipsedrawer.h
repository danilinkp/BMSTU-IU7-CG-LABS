#ifndef ELLIPSEDRAWER_H
#define ELLIPSEDRAWER_H


#include <QPoint>
#include <QList>

class EllipseDrawer
{
private:
	QPoint center;
	int rx;
	int ry;

public:
	EllipseDrawer(QPoint center, int rx, int ry);
	QList<QPoint> canonical();
	QList<QPoint> parametric();
	QList<QPoint> bresenham();
	QList<QPoint> middle_point();
	double time_measurement(QList<QPoint> (EllipseDrawer::*algorithm)());
private:
	void put_symmetrical_points(QList<QPoint> &points, int x, int y);
};


#endif //ELLIPSEDRAWER_H
