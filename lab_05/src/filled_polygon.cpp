#include "filled_polygon.h"
#include <algorithm>
#include <QThread>
#include <QApplication>
#include <QElapsedTimer>
#include <iostream>
#include <limits>

FilledPolygon::FilledPolygon()
{
	contours.emplace_back();
	current_closed = false;
}

void FilledPolygon::add_point(const QPoint &point)
{
	if (current_closed)
	{
		contours.emplace_back();
		current_closed = false;
	}
	contours.back().push_back(point);
}

void FilledPolygon::close_current_contour()
{
	if (contours.back().size() >= 3)
		current_closed = true;
}

void FilledPolygon::clear()
{
	contours.clear();
	contours.emplace_back();
	current_closed = false;
}

bool FilledPolygon::is_valid() const
{
	return !contours.empty() && contours[0].size() >= 3;
}

bool FilledPolygon::is_current_closed() const
{
	return current_closed;
}

void FilledPolygon::add_new_contour()
{
	if (contours.back().empty() || !current_closed)
		return;

	contours.push_back(std::vector<QPoint>());
	current_closed = false;
}

QList<Edge> FilledPolygon::create_edges() const
{
	QList<Edge> edges;
	for (const auto &contour : contours)
	{
		if (contour.size() < 2) continue;
		size_t end_index = (contour == contours.back() && !current_closed) ? contour.size() - 1 : contour.size();
		for (size_t i = 0; i < end_index; ++i)
		{
			QPoint start = contour[i];
			QPoint end = contour[(i + 1) % contour.size()];
			edges.push_back(Edge(start, end));
		}
	}
	return edges;
}

const std::vector<std::vector<QPoint>> &FilledPolygon::get_contours() const
{
	return contours;
}

bool FilledPolygon::has_closed_contours() const
{
	for (const auto &contour : contours)
		if (contour.size() >= 3 && (&contour != &contours.back() || current_closed))
			return true;
	return false;
}

std::pair<int, int> FilledPolygon::find_y_range() const
{
	int y_min = std::numeric_limits<int>::max();
	int y_max = std::numeric_limits<int>::min();

	for (const auto &contour : contours)
	{
		if (contour.size() < 3 || (&contour == &contours.back() && !current_closed))
			continue;

		for (const QPoint &p : contour)
		{
			y_min = std::min(y_min, p.y());
			y_max = std::max(y_max, p.y());
		}
	}

	return {y_min, y_max};
}

std::pair<int, int> FilledPolygon::find_x_range() const
{
	int x_min = std::numeric_limits<int>::max();
	int x_max = std::numeric_limits<int>::min();

	for (const auto &contour : contours)
	{
		if (contour.size() < 3 || (&contour == &contours.back() && !current_closed))
			continue;

		for (const QPoint &p : contour)
		{
			x_min = std::min(x_min, p.x());
			x_max = std::max(x_max, p.x());
		}
	}

	return {x_min, x_max};
}

std::vector<std::pair<int, int>> FilledPolygon::find_x_ranges_per_contour() const
{
	std::vector<std::pair<int, int>> x_ranges;

	for (const auto &contour : contours)
	{
		if (contour.size() < 3 || (&contour == &contours.back() && !current_closed))
		{
			x_ranges.emplace_back(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
			continue;
		}

		int x_min = std::numeric_limits<int>::max();
		int x_max = std::numeric_limits<int>::min();

		for (const QPoint &p : contour)
		{
			x_min = std::min(x_min, p.x());
			x_max = std::max(x_max, p.x());
		}

		x_ranges.emplace_back(x_min, x_max);
	}

	return x_ranges;
}

std::vector<std::tuple<QPoint, QPoint, size_t>> FilledPolygon::collect_edges() const
{
	std::vector<std::tuple<QPoint, QPoint, size_t>> all_edges;

	for (size_t contour_idx = 0; contour_idx < contours.size(); ++contour_idx)
	{
		const auto &contour = contours[contour_idx];
		if (contour.size() < 3 || (&contour == &contours.back() && !current_closed))
			continue;

		for (size_t i = 0; i < contour.size(); ++i)
		{
			QPoint start = contour[i];
			QPoint end = contour[(i + 1) % contour.size()];
			all_edges.emplace_back(start, end, contour_idx);
		}
	}

	return all_edges;
}

void FilledPolygon::fill_polygon(QPainter &painter,
								 const QColor &fill_color,
								 bool with_delay,
								 qint64 &execution_time,
								 QGraphicsScene *scene) const
{
	if (!has_closed_contours())
	{
		execution_time = 0;
		return;
	}

	QElapsedTimer timer;
	timer.start();

	// Находим диапазон Y и X
	auto [y_min, y_max] = find_y_range();
	auto [x_min, x_max] = find_x_range();

	if (y_min == std::numeric_limits<int>::max() || y_max == std::numeric_limits<int>::min() ||
		x_min == std::numeric_limits<int>::max() || x_max == std::numeric_limits<int>::min())
	{
		execution_time = 0;
		return;
	}

	std::vector<std::vector<bool>> frame_buffer = create_frame_buffer(x_min, x_max, y_min, y_max);

	process_edges(frame_buffer, x_min, y_min, x_max, with_delay, scene, painter.device(), fill_color);

	draw_from_buffer(painter, frame_buffer, x_min, y_min, fill_color);

	execution_time = timer.elapsed();
}

std::vector<std::vector<bool>> FilledPolygon::create_frame_buffer(int x_min, int x_max, int y_min, int y_max) const
{
	int width = x_max - x_min + 1;
	int height = y_max - y_min + 1;
	return std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
}

void FilledPolygon::process_edges(std::vector<std::vector<bool>> &frame_buffer,
								  int x_min, int y_min, int /*x_max*/,
								  bool with_delay, QGraphicsScene *scene,
								  QPaintDevice *device, const QColor &fill_color) const
{
	std::vector<std::tuple<QPoint, QPoint, size_t>> edges = collect_edges();

	auto x_ranges = find_x_ranges_per_contour();

	QPixmap temp_pixmap;
	if (with_delay && scene != nullptr && device != nullptr)
	{
		if (auto pixmap_device = dynamic_cast<QPixmap *>(device))
		{
			temp_pixmap = *pixmap_device;
		}
	}

	for (const auto &edge : edges)
	{
		QPoint p1 = std::get<0>(edge);
		QPoint p2 = std::get<1>(edge);
		size_t contour_idx = std::get<2>(edge);

		if (p1.y() == p2.y())
			continue;

		if (p1.y() > p2.y())
			std::swap(p1, p2);

		int contour_x_max = x_ranges[contour_idx].second;

		process_edge_scanlines(frame_buffer, p1, p2, x_min, y_min, contour_x_max,
							   with_delay, scene, device, temp_pixmap, fill_color);
	}
}

void FilledPolygon::process_edge_scanlines(std::vector<std::vector<bool>> &frame_buffer,
										   const QPoint &p1, const QPoint &p2,
										   int x_min, int y_min, int x_max,
										   bool with_delay, QGraphicsScene *scene,
										   QPaintDevice *device, QPixmap &temp_pixmap,
										   const QColor &fill_color) const
{
	int dy = p2.y() - p1.y();
	int dx = p2.x() - p1.x();
	double m = (dx == 0) ? 0 : static_cast<double>(dx) / dy;

	double x = p1.x();

	for (int y = p1.y(); y < p2.y(); ++y)
	{
		int intersect_x = static_cast<int>(x + 0.5);

		invert_pixels_right_of_intersection(frame_buffer, intersect_x, y, x_min, y_min, x_max);

		x += m;

		if (with_delay && scene != nullptr && device != nullptr)
			visualize_with_delay(frame_buffer, x_min, y_min, scene, temp_pixmap, fill_color);
	}
}

void FilledPolygon::invert_pixels_right_of_intersection(std::vector<std::vector<bool>> &frame_buffer,
														int x, int y, int x_min, int y_min, int x_max) const
{
	int buff_y = y - y_min;
	if (buff_y < 0 || buff_y >= static_cast<int>(frame_buffer.size()))
		return;

	for (int i = x; i <= x_max; ++i)
	{
		int buff_x = i - x_min;
		if (buff_x >= 0 && buff_x < static_cast<int>(frame_buffer[buff_y].size()))
		{
			frame_buffer[buff_y][buff_x] = !frame_buffer[buff_y][buff_x];
		}
	}
}

void FilledPolygon::visualize_with_delay(const std::vector<std::vector<bool>> &frame_buffer,
										 int x_min, int y_min, QGraphicsScene *scene, QPixmap &temp_pixmap,
										 const QColor &fill_color) const
{
	QPixmap current_pixmap = temp_pixmap;

	QPainter temp_painter(&current_pixmap);
	temp_painter.setPen(fill_color);

	for (size_t y = 0; y < frame_buffer.size(); ++y)
	{
		for (size_t x = 0; x < frame_buffer[y].size(); ++x)
		{
			if (frame_buffer[y][x])
			{
				temp_painter.drawPoint(x + x_min, y + y_min);
			}
		}
	}
	temp_painter.end();

	scene->clear();
	scene->addPixmap(current_pixmap);

	QApplication::processEvents();
//	QThread::msleep(10);
}

void FilledPolygon::draw_from_buffer(QPainter &painter,
									 const std::vector<std::vector<bool>> &frame_buffer,
									 int x_min, int y_min, const QColor &fill_color) const
{
	painter.setPen(fill_color);
	for (size_t y = 0; y < frame_buffer.size(); ++y)
	{
		for (size_t x = 0; x < frame_buffer[y].size(); ++x)
		{
			if (frame_buffer[y][x])
				painter.drawPoint(x + x_min, y + y_min);
		}
	}
}