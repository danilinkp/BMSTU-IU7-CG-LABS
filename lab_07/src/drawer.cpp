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
	painter.setPen(pen);
	painter.drawLine(point1, point2);
}

void Drawer::draw_rect(const QPoint &point1, const QPoint &point2, const QColor &color)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	QRect rect(point1, point2);
	if (point1.y() <= point2.y())
		rect = QRect(QPoint(point1.x(), point2.y()), QPoint(point2.x(), point1.y()));

	painter.drawRect(rect);
}

void Drawer::clear()
{
	this->pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	QColor color = scene->backgroundBrush().color();
	this->pixmap.fill(color);

	scene->clear();
}

void Drawer::render()
{
	scene->clear();
	scene->addPixmap(this->pixmap);
}
