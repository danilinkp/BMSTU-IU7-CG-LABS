#ifndef DRAWER_H
#define DRAWER_H

#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>

class Drawer
{
public:
	Drawer(QGraphicsScene *scene, QPixmap pixmap);
	void draw_line(const QPoint &point1, const QPoint &point2, const QColor &color);
	void clear();
	void render();

private:
	QGraphicsScene *scene;
	QPixmap pixmap;
};


#endif //DRAWER_H
