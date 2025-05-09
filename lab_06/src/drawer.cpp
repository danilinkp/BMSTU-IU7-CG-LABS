#include "drawer.h"

#include <utility>

Drawer::Drawer(QGraphicsScene *scene, QPixmap pixmap)
{
	this->scene = scene;
	this->pixmap = std::move(pixmap);
}

void Drawer::draw_pixel(const QPoint &point, const QColor &color)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	painter.drawPoint(point);
}

void Drawer::draw_pixel(int x, int y, const QColor &color)
{
	QPoint point(x, y);
	draw_pixel(point, color);
}

void Drawer::draw_line(const QPoint &point1, const QPoint &point2, const QColor &color)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	painter.drawLine(point1, point2);
}

void Drawer::draw_figure(const Figure &figure, const QColor &color)
{
	for (size_t i = 1; i < figure.get_points_count(); i++)
		draw_line(figure.get_point(i - 1), figure.get_point(i), color);

	if (figure.is_closed())
		draw_line(figure.get_point(figure.get_points_count() - 1), figure.get_point(0), color);
}

void Drawer::draw_figures(const std::vector<Figure> &figures, const QColor &color)
{
	for (const auto & figure : figures)
		draw_figure(figure, color);
}

QColor Drawer::get_pixel_color(const QPoint &point)
{
	return pixmap.toImage().pixelColor(point);
}

QColor Drawer::get_pixel_color(int x, int y)
{
	QPoint point(x, y);
	return get_pixel_color(point);
}

size_t Drawer::get_pixmap_width()
{
	return pixmap.width();
}

size_t Drawer::get_pixmap_height()
{
	return pixmap.height();
}

void Drawer::clear()
{
	this->pixmap = QPixmap(scene->width(), scene->height());
	QColor color = scene->backgroundBrush().color();
	this->pixmap.fill(color);

	scene->clear();
}

void Drawer::render()
{
	scene->clear();
	scene->addPixmap(this->pixmap);
}