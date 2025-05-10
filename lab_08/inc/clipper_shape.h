#ifndef CLIPPER_SHAPE_H
#define CLIPPER_SHAPE_H

#include "line_segment.h"
#include "drawer.h"

class ClipperShape
{
public:
	virtual ~ClipperShape() = default;
	virtual void draw(Drawer &drawer, const QColor &color) const = 0;
	virtual LineSegment clip(const LineSegment &line) = 0;
};


#endif //CLIPPER_SHAPE_H
