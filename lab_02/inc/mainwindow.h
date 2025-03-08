#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graphicsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_move_button_clicked();
	void on_scale_button_clicked();
	void on_rotate_button_clicked();
	void on_reset_button_clicked();
	void on_back_button_clicked();
	void show_center_figure();

private:
	Ui::MainWindow *ui;
	GraphicsManager *graphicsManager;
};

#endif // MAINWINDOW_H