#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("1-ая лаба");
	scene = new QGraphicsScene();

	auto validator = new QDoubleValidator;
	validator->setLocale(QLocale::C);
	ui->x_line_edit->setValidator(validator);
	ui->y_line_edit->setValidator(validator);

	ui->graphics_view->setScene(scene);

	ui->coords_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui->task_push_button, &QPushButton::clicked, this, &MainWindow::show_task);
	connect(ui->add_point_push_button, &QPushButton::clicked, this, &MainWindow::add_point);
}

MainWindow::~MainWindow()
{
	delete ui;

	delete scene;
}

void MainWindow::add_point()
{

}

void MainWindow::show_task()
{
	QMessageBox::information(nullptr,
							 "Условие задачи",
							 "Найти треугольник, у которого угол, образованный высотой и медианной, исходящие из одного угла, минимален");
}
