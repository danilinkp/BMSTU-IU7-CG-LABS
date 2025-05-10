#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "drawer.h"
#include "clipper.h"
#include "clipper_rectangle.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow: public QMainWindow
{
Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

protected:
	void mousePressEvent(QMouseEvent *event) override;

private slots:
	void on_clear_btn_clicked();
	void on_change_section_color_btn_clicked();
	void on_change_clipper_color_btn_clicked();
	void on_change_result_color_btn_clicked();
	void on_clip_btn_clicked();
	void on_draw_clipper_btn_clicked();
	void on_draw_section_btn_clicked();

private:
	static void set_preview_color(QWidget *widget, const QColor &color);
	void redraw_scene();

private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;

	Drawer *drawer;

	QColor section_color = Qt::blue;
	QColor clipper_color = Qt::black;
	QColor result_color = Qt::red;

	enum class InputMode { None, SegmentFirst, SegmentSecond, ClipperFirst, ClipperSecond };
	InputMode input_mode = InputMode::None;
	QPoint first_point;
	std::vector<LineSegment> segments;
	ClipperRectangle *clipper = nullptr;
	std::vector<LineSegment> clipped_segments;
};

#endif // MAINWINDOW_H