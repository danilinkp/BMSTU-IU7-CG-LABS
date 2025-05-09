#ifndef EDGE_H
#define EDGE_H

#include <QPoint>

class Edge
{
public:
	QPoint start, end;

	Edge(const QPoint &s, const QPoint &e);
	[[nodiscard]] bool is_horizontal() const;
	[[nodiscard]] bool is_vertical() const;
};


#endif //EDGE_H
