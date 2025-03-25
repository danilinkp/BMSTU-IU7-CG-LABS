#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "circledrawer.h"
#include "ellipsedrawer.h"
#include <QMainWindow>
#include <QGraphicsScene>

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

private slots:
	void on_change_back_color_btn_clicked();
	void on_change_figure_color_btn_clicked();
	void on_draw_circle_btn_clicked();
	void on_draw_ellipse_btn_clicked();
	void on_draw_circle_spectra_btn_clicked();
	void on_draw_ellipse_spectra_btn_clicked();
	void on_clear_btn_clicked();
	void on_circle_time_cmp_btn_clicked();
	void on_ellipse_time_cmp_btn_clicked();
	void on_back_btn_clicked();
	void on_back_btn_2_clicked();
private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;

	QColor background_color = Qt::white;
	QColor figure_color = Qt::red;

	static void set_preview_color(QWidget *widget, const QColor &color);
	void show_err_msg(const QString &msg);
	void draw_circle_by_algorithm(const QString &algorithm, const QPoint &center, int radius);
	void draw_figure(const QColor &color, const QPoint &center, int radius);
	void draw_figure(const QColor &color, const QPoint &center, int rx, int ry);
	void draw_figure(const QColor &color, const QList<QPoint> &figure);
	void add_figure_to_scene();
	void draw_ellipse_by_algorithm(const QString &algorithm, const QPoint &center, int rx, int ry);
	double lib_time_measurement(const QPoint &center, int radius);

	double lib_time_measurement(const QPoint &center, int rx, int ry);
};

#endif // MAINWINDOW_H