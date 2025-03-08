#include "graphicsmanager.h"
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <cmath>
#include <QAbstractGraphicsShapeItem>

const double PARABOLA_WIDTH_COEF = 10;

const double PARABOLA_MAX = 99999.0;

GraphicsManager::GraphicsManager(QObject *parent)
	: QObject(parent), graphicsView(nullptr), scene(new QGraphicsScene(this))
{}

void GraphicsManager::setGraphicsView(QGraphicsView *view)
{
	graphicsView = view;
	graphicsView->setScene(scene);
	graphicsView->setRenderHint(QPainter::Antialiasing);
}

void GraphicsManager::drawInitialFigure()
{
	scene->clear();
	double a = 0, b = 0, c = 0, d = 0, r = 200;

	setupParabola(a, c, d, r, parabola);
	setupCircle(360, r, a, -b, circle);
	intersection = parabola.intersected(circle);

	redraw();
	history.clear();
	history.push({"initial", parabola, circle, intersection, 0, 0, 0, 0});
}

void GraphicsManager::setupParabola(double a, double c, double d, double r, QPolygonF &polygon)
{
	polygon.clear();
	double x = a + r + PARABOLA_MAX - 1;
	double y = sqrt(x - c) + d;
	while (x < a + r + PARABOLA_MAX)
	{
		x = c + pow(y - d, 2);
		polygon.append(QPointF(x, -(y * PARABOLA_WIDTH_COEF - d)));
		y -= 0.1;
	}
}

void GraphicsManager::setupCircle(double n, double r, double x_pos, double y_pos, QPolygonF &polygon)
{
	polygon.clear();
	double w = 360.0 / n;
	for (int i = 0; i < n; ++i)
	{
		double t = w * i;
		double x = r * cos(t * M_PI / 180.0);
		double y = r * sin(t * M_PI / 180.0);
		polygon.append(QPointF(x_pos + x, y_pos - y));
	}
}

QPolygonF GraphicsManager::rotatePolygon(const QPolygonF &polygon, double alpha, double rx, double ry)
{
	QPolygonF result;
	double cosA = cos(alpha * M_PI / 180.0);
	double sinA = sin(alpha * M_PI / 180.0);

	for (const QPointF &point : polygon)
	{
		double x = point.x() - rx;
		double y = point.y() + ry;
		double newX = x * cosA + y * sinA + rx;
		double newY = -x * sinA + y * cosA - ry;
		result.append(QPointF(newX, newY));
	}
	return result;
}

QPolygonF GraphicsManager::scalePolygon(const QPolygonF &polygon, double kx, double ky, double cx, double cy)
{
	QPolygonF result;

	for (const QPointF &point : polygon)
	{
		double x = (point.x() - cx) * kx + cx;
		double y = (point.y() + cy) * ky - cy;
		result.append(QPointF(x, y));
	}
	return result;
}

QPolygonF GraphicsManager::movePolygon(const QPolygonF &polygon, double x, double y)
{
	QPolygonF result;
	for (const QPointF &point : polygon)
		result.append(QPointF(point.x() + x, point.y() - y));
	return result;
}

void GraphicsManager::redraw()
{
	scene->clear();
	QPen blackPen(Qt::black);
	blackPen.setCosmetic(true);
	QPen redPen(Qt::red);
	redPen.setCosmetic(true);
	QPen bluePen(Qt::blue);
	bluePen.setCosmetic(true);

	double centerX = graphicsView->width() / 2.0;
	double centerY = graphicsView->height() / 2.0;

	QPolygonF shiftedParabola = parabola.translated(centerX, centerY);
	QPolygonF shiftedCircle = circle.translated(centerX, centerY);
	QPolygonF shiftedIntersection = intersection.translated(centerX, centerY);

	auto *parabolaItem = new QGraphicsPolygonItem(shiftedParabola);
	parabolaItem->setPen(blackPen);
	scene->addItem(parabolaItem);

	auto *circleItem = new QGraphicsPolygonItem(shiftedCircle);
	circleItem->setPen(redPen);
	scene->addItem(circleItem);

	auto *intersectionItem = new QGraphicsPolygonItem(shiftedIntersection);
	intersectionItem->setPen(bluePen);

	intersectionItem->setBrush(QBrush(Qt::blue, Qt::BDiagPattern));
	scene->addItem(intersectionItem);

	QPointF center = get_intersection_center();
	QPointF shiftedCenter(center.x() + centerX, center.y() + centerY);
	qreal pointSize = 5.0;
	auto *centerPoint = new QGraphicsEllipseItem(
		shiftedCenter.x() - pointSize / 2,
		shiftedCenter.y() - pointSize / 2,
		pointSize,
		pointSize
	);
	centerPoint->setPen(QPen(Qt::black));
	centerPoint->setBrush(QBrush(Qt::black));
	scene->addItem(centerPoint);

	scene->setSceneRect(0, 0, graphicsView->width(), graphicsView->height());
}

void GraphicsManager::move(double dx, double dy)
{
	history.push({"move", parabola, circle, intersection, dx, dy, 0, 0});
	parabola = movePolygon(parabola, dx, dy);
	circle = movePolygon(circle, dx, dy);
	intersection = parabola.intersected(circle);
	redraw();
}

void GraphicsManager::scale(double kx, double ky, double cx, double cy)
{
	history.push({"scale", parabola, circle, intersection, kx, ky, cx, cy});
	parabola = scalePolygon(parabola, kx, ky, cx, cy);
	circle = scalePolygon(circle, kx, ky, cx, cy);
	intersection = parabola.intersected(circle);
	redraw();
}

void GraphicsManager::rotate(double angle, double rx, double ry)
{
	history.push({"rotate", parabola, circle, intersection, angle, rx, ry, 0});
	parabola = rotatePolygon(parabola, angle, rx, ry);
	circle = rotatePolygon(circle, angle, rx, ry);
	intersection = parabola.intersected(circle);
	redraw();
}

void GraphicsManager::reset()
{
	history.clear();
	drawInitialFigure();
}

void GraphicsManager::undo()
{
	if (!history.empty())
	{
		auto [type, oldParabola, oldCircle, oldIntersection, p1, p2, p3, p4] = history.pop();
		parabola = oldParabola;
		circle = oldCircle;
		intersection = oldIntersection;
		redraw();
	}
}

QPointF GraphicsManager::get_intersection_center()
{
	if (intersection.isEmpty())
	{
		return {0, 0};
	}

	qreal area = 0;
	qreal cx = 0;
	qreal cy = 0;
	int n = intersection.size();

	for (int i = 0; i < n; i++)
	{
		const QPointF &p1 = intersection[i];
		const QPointF &p2 = intersection[(i + 1) % n];
		qreal cross = (p1.x() * p2.y() - p2.x() * p1.y());
		area += cross;
		cx += (p1.x() + p2.x()) * cross;
		cy += (p1.y() + p2.y()) * cross;
	}

	area /= 2.0;
	if (qAbs(area) < 1e-10)
		return {0, 0};

	cx /= (6.0 * area);
	cy /= (6.0 * area);

	cx = fabs(cx) < 1e-4 ? 0 : cx;
	cy = fabs(cy) < 1e-4 ? 0 : cy;

	return {cx, cy};
}
