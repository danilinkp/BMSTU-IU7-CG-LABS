#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "linedrawer.h"

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
	void on_change_line_color_btn_clicked();
	void on_clear_btn_clicked();
	void on_draw_line_btn_clicked();
	void on_draw_spectre_btn_clicked();
	void on_time_cmp_btn_clicked();
	void on_step_cmp_btn_clicked();
	void on_back_btn_clicked();
	void on_back_btn_2_clicked();
	void on_time_measurement_btn_clicked();

private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;

	QColor background_color = Qt::white;
	QColor line_color = Qt::red;

	static void set_preview_color(QWidget *widget, const QColor &color);
	void show_err_msg(const QString &msg);
	void draw_line_by_algorithm(const QString &algorithm, QPoint start, QPoint end);
	void draw_line(const QColor &color, const QPoint &start, const QPoint &end);
	void add_line_to_scene();
	void draw_line(QColor color, const QList<Pixel> &line);
	double library_algorithm_time(const QPoint &start, const QPoint &end);
};

#endif // MAINWINDOW_H