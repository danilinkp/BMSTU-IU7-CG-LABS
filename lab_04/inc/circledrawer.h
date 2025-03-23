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

};

#endif //CIRCLEDRAWER_H
