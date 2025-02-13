#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	scene = new QGraphicsScene();

	auto validator = new QDoubleValidator;
	validator->setLocale(locale);
	ui->x_line_edit->setValidator(validator);
	ui->y_line_edit->setValidator(validator);

	ui->graphics_view->setScene(scene);

	ui->coords_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui->answer_push_button, &QPushButton::clicked, this, &MainWindow::show_task);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::show_task()
{
	QMessageBox::information(nullptr, "Условие задачи",
							 "Найти треугольник, у которого угол, образованный высотой и медианной минимален");
}
