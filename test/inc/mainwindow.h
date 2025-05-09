#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "polygon.h"
#include "filler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_draw_point_btn_clicked();
	void on_close_figure_btn_clicked();
	void on_clear_btn_clicked();
	void on_draw_btn_clicked();
	void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::MainWindow *ui;
	Polygon polygon;
	Filler filler;
	bool figure_closed = false;

	void drawPolygon();
	void clearScreen();
	void fillPolygon();
};

#endif // MAINWINDOW_H
