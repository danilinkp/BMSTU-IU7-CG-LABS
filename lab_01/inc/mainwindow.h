#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QMessageBox>

#include "point.h"
#include "triangle.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QVector<Point> points;

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	static void show_task();
	void add_point();
	void read_table();
	void remove_row_from_table(int row);
	void del_point();
	void del_last_point();
	void del_all_points();
	Triangle best_triangle();
	void show_result();
};
#endif // MAINWINDOW_H