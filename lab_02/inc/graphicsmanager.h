#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStack>
#include <QTransform>
#include <QPolygonF>

class GraphicsManager: public QObject
{
Q_OBJECT
public:
	explicit GraphicsManager(QObject *parent = nullptr);
	void set_graphics_view(QGraphicsView *view);
	void draw_initial_figure();
	void move(double dx, double dy);
	void scale(double kx, double ky, double cx, double cy);
	void rotate(double angle, double rx, double ry);
	void reset();
	void undo();
	QPointF get_intersection_center();

private:
	QGraphicsView *graphicsView;
	QGraphicsScene *scene;
	QPolygonF parabola, circle, intersection;
	QStack<std::tuple<QString, QPolygonF, QPolygonF, QPolygonF, double, double, double, double>> history;
	static void setup_parabola(double a, double c, double d, double r, QPolygonF &polygon);
	static void setup_circle(double n, double r, double x_pos, double y_pos, QPolygonF &polygon);
	static QPolygonF rotate_polygon(const QPolygonF &polygon, double alpha, double rx, double ry);
	static QPolygonF scale_polygon(const QPolygonF &polygon, double kx, double ky, double cx, double cy);
	static QPolygonF move_polygon(const QPolygonF &polygon, double x, double y);
	void redraw();
};

#endif // GRAPHICSMANAGER_H