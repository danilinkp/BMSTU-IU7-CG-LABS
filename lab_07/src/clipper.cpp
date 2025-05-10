#include "clipper.h"


Clipper::Clipper(ClipperShape *shape)
	: shape(shape)
{}

std::vector<LineSegment> Clipper::clip_segments(const std::vector<LineSegment> &segments) const
{
	std::vector<LineSegment> clippedSegments;
	for (const auto &segment : segments)
	{
		LineSegment clipped = shape->clip(segment);
		if (clipped.start().x() != clipped.end().x() || clipped.start().y() != clipped.end().y())
			clippedSegments.push_back(clipped);
	}
	return clippedSegments;
}