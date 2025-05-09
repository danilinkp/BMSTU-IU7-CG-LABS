#ifndef DRAWER_H
#define DRAWER_H

#include "figure.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>

class Drawer
{
public:
	Drawer(QGraphicsScene *scene, QPixmap pixmap);
	void draw_pixel(int x, int y, const QColor &color);
	void draw_pixel(const QPoint &point, const QColor &color);
	void draw_line(const QPoint &point1, const QPoint &point2, const QColor &color);
	void draw_figure(const Figure &figure, const QColor &color);
	void draw_figures(const std::vector<Figure> &figures, const QColor &color);
	QColor get_pixel_color(const QPoint &point);
	QColor get_pixel_color(int x, int y);
	size_t get_pixmap_width();
	size_t get_pixmap_height();
	void clear();
	void render();

private:
	QGraphicsScene *scene;
	QPixmap pixmap;
};


#endif //DRAWER_H
