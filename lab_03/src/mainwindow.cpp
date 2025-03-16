#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	scene = new QGraphicsScene;
	setWindowTitle("3-ая лаба");
	setMinimumSize(1340, 750);
//	showMaximized();

}

MainWindow::~MainWindow()
{
	delete ui;
	delete scene;
}