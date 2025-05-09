#include "edge.h"

Edge::Edge(const QPoint &s, const QPoint &e)
	: start(s), end(e)
{}

bool Edge::is_horizontal() const
{
	return start.y() == end.y();
}

bool Edge::is_vertical() const
{
	return start.x() == end.x();
}