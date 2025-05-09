#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "filled_polygon.h"

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
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private slots:
	void on_draw_point_btn_clicked();
	void on_close_figure_btn_clicked();
	void on_clear_btn_clicked();
	void on_draw_btn_clicked();
	void on_change_back_color_btn_clicked();
	void on_change_figure_color_btn_clicked();
	void on_delay_radio_btn_toggled(bool checked);
	void on_without_delay_radio_btn_toggled(bool checked);

private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;
	QPixmap filled_pixmap;
	FilledPolygon polygon;
	QColor background_color = Qt::white;
	QColor figure_color = Qt::green;
	bool delay_mode = false;

	void set_preview_color(QWidget* widget, const QColor& color);
	void draw_polygon();
	void update_points_table();
	void update_scene();
};

#endif // MAINWINDOW_H
