
#include "line_segment.h"

LineSegment::LineSegment(const QPoint &start, const QPoint &end)
{
	m_start = start;
	m_end = end;
}

QPoint LineSegment::start() const
{
	return m_start;
}

QPoint LineSegment::end() const
{
	return m_end;
}

void LineSegment::set_start(const QPoint &start)
{
	m_start = start;
}

void LineSegment::set_end(const QPoint &end)
{
	m_end = end;
}