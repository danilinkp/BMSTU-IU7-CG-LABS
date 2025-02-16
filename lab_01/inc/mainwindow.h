#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QGraphicsTextItem>

#include "point.h"
#include "triangle.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QVector<Point> points;

private:
	static void show_task();
	void add_point();
	void read_table();
	void remove_row_from_table(int row);
	void del_point();
	void del_last_point();
	void del_all_points();
	Triangle best_triangle();
	void draw_triangle(const Triangle &triangle, const QPen &pen);
	void draw_points(const Triangle &triangle, double point_size, const QPen &pen);
	void set_point_text(const Point &point, const QString &string, const QFont &font, const QTransform &transform);
	void draw_result(const Triangle &triangle);
	void show_result();
};
#endif // MAINWINDOW_H