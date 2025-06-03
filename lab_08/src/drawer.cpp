#include "drawer.h"

#include <utility>

Drawer::Drawer(QGraphicsScene *scene, QPixmap pixmap)
{
	this->scene = scene;
	this->pixmap = std::move(pixmap);
}

void Drawer::draw_line(const QPoint &point1, const QPoint &point2, const QColor &color)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	pen.setWidth(2);
	painter.setPen(pen);
	painter.drawLine(point1, point2);
}

void Drawer::clear()
{
	this->pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()) + 20);
	QColor color = scene->backgroundBrush().color();
	this->pixmap.fill(color);

	scene->clear();
}

void Drawer::render()
{
	scene->clear();
	scene->addPixmap(this->pixmap);
}
