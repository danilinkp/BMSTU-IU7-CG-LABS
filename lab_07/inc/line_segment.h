#ifndef LINE_SEGMENT_H
#define LINE_SEGMENT_H

#include <QPoint>

class LineSegment
{
public:
	LineSegment(const QPoint &start, const QPoint &end);
	[[nodiscard]] QPoint start() const;
	[[nodiscard]] QPoint end() const;

	void set_start(const QPoint &start);
	void set_end(const QPoint &end);
private:
	QPoint m_start, m_end;
};


#endif //LINE_SEGMENT_H
