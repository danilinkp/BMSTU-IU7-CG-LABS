#include "clipper_rectangle.h"

ClipperRectangle::ClipperRectangle(const QPoint &left, const QPoint &right)
{
	m_left = left;
	m_right = right;
}

bool ClipperRectangle::contains(const QPoint &point) const
{
	if (m_left.y() >= m_right.y())
		return point.x() >= m_left.x() && point.x() <= m_right.x() && point.y() <= m_left.y()
			&& point.y() >= m_right.y();
	else
		return point.x() >= m_left.x() && point.x() <= m_right.x() && point.y() >= m_left.y()
			&& point.y() <= m_right.y();
}

void ClipperRectangle::draw(Drawer &drawer, const QColor &color) const
{
	drawer.draw_rect(m_left, m_right, color);
	drawer.render();
}

int ClipperRectangle::get_mask(const QPoint &point) const
{
	int result = 0;

	int x_l = std::min(m_left.x(), m_right.x());
	int x_r = std::max(m_left.x(), m_right.x());
	int y_t = std::max(m_left.y(), m_right.y());
	int y_b = std::min(m_left.y(), m_right.y());

	if (point.x() < x_l)
		result |= left;
	else if (point.x() > x_r)
		result |= right;

	if (point.y() > y_t)
		result |= higher;
	else if (point.y() < y_b)
		result |= lower;

	return result;
}

LineSegment ClipperRectangle::clip(const LineSegment &segment) const
{
	QPoint p1 = segment.start(), p2 = segment.end();
	QPoint r1, r2;

	int T1 = get_mask(p1), T2 = get_mask(p2);
	double line_tan = 1e30;

	if (!T1 && !T2)
		return segment;
	if (T1 & T2)
		return {QPoint(0, 0), QPoint(0, 0)};

	int x_left = std::min(m_left.x(), m_right.x()) - 1;
	int x_right = std::max(m_left.x(), m_right.x()) + 1;
	int y_top = std::max(m_left.y(), m_right.y()) - 1;
	int y_bottom = std::min(m_left.y(), m_right.y()) + 1;

	// Шаги 9-10: Проверка видимости концов отрезка
	QPoint Q;
	int i = 0;

	if (!T1)
	{
		r1 = p1;
		Q = p2;
		i = 1;
	}
	else if (!T2)
	{
		r1 = p2;
		Q = p1;
		i = 1;
	}
	else
	{
		i = 0;
		Q = p1;
	}

	// Шаги 12-29: Цикл отсечения
	while (i < 2)
	{
		i++;

		// Шаг 15: Проверка вертикальности отрезка
		bool is_vertical = (p1.x() == p2.x());

		// Шаг 16: Вычисление тангенса угла наклона отрезка
		if (!is_vertical)
			line_tan = static_cast<double>(p2.y() - p1.y()) / (p2.x() - p1.x());

		// Обработка пересечений с левой и правой границами
		if (!is_vertical)
		{
			// Шаги 17-19: Проверка левой границы
			if (Q.x() < x_left)
			{
				double y_intersection = line_tan * (x_left - Q.x()) + Q.y();
				if (y_intersection >= y_bottom && y_intersection <= y_top)
				{
					if (i == 1)
						r1 = QPoint(x_left, static_cast<int>(y_intersection));
					else
						r2 = QPoint(x_left, static_cast<int>(y_intersection));
					Q = (Q == p1) ? p2 : p1;
					continue;
				}
			}

			// Шаги 20-22: Проверка правой границы
			if (Q.x() > x_right)
			{
				double y_intersection = line_tan * (x_right - Q.x()) + Q.y();
				if (y_intersection >= y_bottom && y_intersection <= y_top)
				{
					if (i == 1)
						r1 = QPoint(x_right, static_cast<int>(y_intersection));
					else
						r2 = QPoint(x_right, static_cast<int>(y_intersection));
					Q = (Q == p1) ? p2 : p1;
					continue;
				}
			}
		}

		// Шаг 23: Проверка горизонтальности отрезка
		bool is_horizontal = (line_tan == 0);
		if (is_horizontal)
		{
			Q = (Q == p1) ? p2 : p1;
			continue;
		}

		// Шаги 24-26: Проверка верхней границы
		if (Q.y() > y_top)
		{
			double x_intersection = (y_top - Q.y()) / line_tan + Q.x();
			if (x_intersection >= x_left && x_intersection <= x_right)
			{
				if (i == 1)
					r1 = QPoint(static_cast<int>(x_intersection), y_top);
				else
					r2 = QPoint(static_cast<int>(x_intersection), y_top);
				Q = (Q == p1) ? p2 : p1;
				continue;
			}
		}

		// Шаги 27-29: Проверка нижней границы
		if (Q.y() < y_bottom)
		{
			double x_intersection = (y_bottom - Q.y()) / line_tan + Q.x();
			if (x_intersection >= x_left && x_intersection <= x_right)
			{
				if (i == 1)
					r1 = QPoint(static_cast<int>(x_intersection), y_bottom);
				else
					r2 = QPoint(static_cast<int>(x_intersection), y_bottom);
				Q = (Q == p1) ? p2 : p1;
				continue;
			}
		}

		return {QPoint(0, 0), QPoint(0, 0)};
	}

	return {r1, r2};
}
