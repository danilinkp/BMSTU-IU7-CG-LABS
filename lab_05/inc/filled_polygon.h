#ifndef FILLED_POLYGON_H
#define FILLED_POLYGON_H

#include <QPoint>
#include <QPainter>
#include <QGraphicsScene>
#include <vector>

struct Edge {
	QPoint start;
	QPoint end;
	Edge(const QPoint &s, const QPoint &e) : start(s), end(e) {}
};

class FilledPolygon {
public:
	FilledPolygon();

	void add_point(const QPoint &point);
	void close_current_contour();
	void clear();
	bool is_valid() const;
	bool is_current_closed() const;
	void add_new_contour();
	QList<Edge> create_edges() const;
	const std::vector<std::vector<QPoint>> &get_contours() const;
	bool has_closed_contours() const;
	std::pair<int, int> find_y_range() const;
	std::pair<int, int> find_x_range() const;
	std::vector<std::pair<int, int>> find_x_ranges_per_contour() const;
	std::vector<std::tuple<QPoint, QPoint, size_t>> collect_edges() const;

	void fill_polygon(QPainter &painter, const QColor &fill_color, bool with_delay,
					  qint64 &execution_time, QGraphicsScene *scene = nullptr) const;

private:
	std::vector<std::vector<QPoint>> contours;
	bool current_closed;

	std::vector<std::vector<bool>> create_frame_buffer(int x_min, int x_max, int y_min, int y_max) const;
	void process_edges(std::vector<std::vector<bool>> &frame_buffer, int x_min, int y_min, int x_max,
					   bool with_delay, QGraphicsScene *scene, QPaintDevice *device,
					   const QColor &fill_color) const;
	void process_edge_scanlines(std::vector<std::vector<bool>> &frame_buffer, const QPoint &p1, const QPoint &p2,
								int x_min, int y_min, int x_max, bool with_delay, QGraphicsScene *scene,
								QPaintDevice *device, QPixmap &temp_pixmap, const QColor &fill_color) const;
	void invert_pixels_right_of_intersection(std::vector<std::vector<bool>> &frame_buffer, int x, int y,
											 int x_min, int y_min, int x_max) const;
	void visualize_with_delay(const std::vector<std::vector<bool>> &frame_buffer, int x_min, int y_min,
							  QGraphicsScene *scene, QPixmap &temp_pixmap,
							  const QColor &fill_color) const;
	void draw_from_buffer(QPainter &painter, const std::vector<std::vector<bool>> &frame_buffer,
						  int x_min, int y_min, const QColor &fill_color) const;
};

#endif // FILLED_POLYGON_H