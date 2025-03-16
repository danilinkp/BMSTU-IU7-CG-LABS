#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include <QPoint>
#include <QList>

class Pixel
{
private:
	QPoint point;
	int intensity;
public:
	explicit Pixel(QPoint point)
		: point(point), intensity(255)
	{};
	Pixel(QPoint point, int intensity)
		: point(point), intensity(intensity)
	{};
};

class LineDrawer
{
private:
	QPoint start;
	QPoint end;

public:
	LineDrawer(QPoint first, QPoint second);

	QList<Pixel> dda();
	QList<Pixel> bresenham_int();
	QList<Pixel> bresenham_float();
	QList<Pixel> bresenham_smooth();
	QList<Pixel> bresenham_smooth(int max_intensity);
	QList<Pixel> wu();
	QList<Pixel> wu(int max_intensity);

private:
	static int sign(double value);
};

#endif //LINEDRAWER_H
