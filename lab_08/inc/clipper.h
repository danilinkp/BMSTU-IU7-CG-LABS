#ifndef CLIPPER_H
#define CLIPPER_H

#include <vector>
#include "clipper_shape.h"
#include "line_segment.h"

class Clipper
{
public:
	explicit Clipper(ClipperShape *shape);
	[[nodiscard]] std::vector<LineSegment> clip_segments(const std::vector<LineSegment> &segments) const;

private:
	ClipperShape *shape;
};


#endif //CLIPPER_H
