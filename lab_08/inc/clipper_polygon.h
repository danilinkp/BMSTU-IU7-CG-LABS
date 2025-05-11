#ifndef CLIPPER_POLYGON_H
#define CLIPPER_POLYGON_H

#include "clipper_shape.h"

class ClipperPolygon: public ClipperShape
{
public:
	explicit ClipperPolygon(const std::vector<QPoint> &points);
	void draw(Drawer &drawer, const QColor &color) const override;
	[[nodiscard]] LineSegment clip(const LineSegment &segment) override;
	bool is_convex();

private:
	std::vector<QPoint> points;

private:
	static QPoint get_directrice(const QPoint &point_1, const QPoint &point_2);
	static int scalar_product(const QPoint &vector_1, const QPoint &vector_2);
	static int cross_product(const QPoint &vector_1, const QPoint &vector_2);
	static QPoint get_normal(const QPoint &point_1, const QPoint &point_2, const QPoint &point_3);
};


#endif //CLIPPER_POLYGON_H
