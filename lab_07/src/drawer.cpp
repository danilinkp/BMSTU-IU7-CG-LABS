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

	int x_left = std::min(point1.x(), point2.x());
	int x_right = std::max(point1.x(), point2.x());
	int y_top = std::min(point1.y(), point2.y());
	int y_bottom = std::max(point1.y(), point2.y());

	painter.drawLine(x_left, y_top, x_left, y_bottom);
	painter.drawLine(x_left, y_top, x_right, y_top);
	painter.drawLine(x_right, y_top, x_right, y_bottom);
	painter.drawLine(x_right, y_bottom, x_left, y_bottom);
}

void Drawer::fill_rectangle(const QRectF &rect, const QColor &color)
{
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(color));
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
