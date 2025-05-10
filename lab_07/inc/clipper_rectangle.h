#ifndef CLIPPER_RECTANGLE_H
#define CLIPPER_RECTANGLE_H

#include "clipper_shape.h"

class ClipperRectangle: public ClipperShape
{
public:
	ClipperRectangle(const QPoint &left, const QPoint &right);
	[[nodiscard]] bool contains(const QPoint &point) const override;
	void draw(Drawer &drawer, const QColor &color) const override;
	[[nodiscard]] LineSegment clip(const LineSegment &segment) const override;
private:
	QPoint m_left, m_right;
private:
	[[nodiscard]] int get_mask(const QPoint &point) const;

	static const int higher = 0b1000;
	static const int lower = 0b0100;
	static const int right = 0b0010;
	static const int left = 0b0001;
};


#endif //CLIPPER_RECTANGLE_H
