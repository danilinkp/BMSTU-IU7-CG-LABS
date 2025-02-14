#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	this->setWindowTitle("1-ая лаба");
	this->setFixedSize(1000, 750);
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	ui->graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	scene = new QGraphicsScene();

	auto validator = new QDoubleValidator;
	validator->setLocale(QLocale::C);
	ui->x_line_edit->setValidator(validator);
	ui->y_line_edit->setValidator(validator);

	ui->graphics_view->setScene(scene);

	ui->coords_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui->task_push_button, &QPushButton::clicked, this, &MainWindow::show_task);
	connect(ui->add_point_push_button, &QPushButton::clicked, this, &MainWindow::add_point);
	connect(ui->del_point_push_button, &QPushButton::clicked, this, &MainWindow::del_point);
	connect(ui->del_last_point_push_button, &QPushButton::clicked, this, &MainWindow::del_last_point);
	connect(ui->del_all_points_push_button, &QPushButton::clicked, this, &MainWindow::del_all_points);
	connect(ui->answer_push_button, &QPushButton::clicked, this, &MainWindow::show_result);
}

MainWindow::~MainWindow() {
	delete ui;
	delete scene;
}

void MainWindow::read_table() {
	if (!points.empty())
		points.clear();

	for (int i = 0; i < ui->coords_table_widget->rowCount(); i++) {

		Point p(ui->coords_table_widget->item(i, 0)->text().toDouble(),
				ui->coords_table_widget->item(i, 1)->text().toDouble());
		points.append(p);
	}
}

void MainWindow::add_point() {
	bool is_x_converted, is_y_converted;
	double x = ui->x_line_edit->text().toDouble(&is_x_converted);
	double y = ui->y_line_edit->text().toDouble(&is_y_converted);

	if (!is_x_converted || !is_y_converted)
		ui->answer_text_edit->setPlainText("Координаты точки должны быть вещественными.");
	else {
		int rows = ui->coords_table_widget->rowCount();
		ui->coords_table_widget->insertRow(rows);
		ui->coords_table_widget->setItem(rows, 0, new QTableWidgetItem(QString::number(x, 'f')));
		ui->coords_table_widget->setItem(rows, 1, new QTableWidgetItem(QString::number(y, 'f')));

		ui->answer_text_edit->setPlainText
			("Точка (" + QString::number(x) + "; " + QString::number(y) + ") успешно добавлена.");
	}
}

void MainWindow::remove_row_from_table(int row) {
	QString x = ui->coords_table_widget->item(row, 0)->text();
	QString y = ui->coords_table_widget->item(row, 1)->text();

	ui->coords_table_widget->removeRow(row);

	ui->answer_text_edit->setPlainText("Точка (" + x + "; " + y + ") успешно удалена.");
}

void MainWindow::del_point() {
	int row = ui->point_spin_box->value() - 1;
	if (row < 0 || row >= ui->coords_table_widget->rowCount())
		ui->answer_text_edit->setPlainText("Ошибка такой точки не существует.");
	else
		remove_row_from_table(row);
}

void MainWindow::del_last_point() {
	int rows = ui->coords_table_widget->rowCount();
	if (!rows)
		ui->answer_text_edit->setPlainText("Таблица пуста.");
	else
		remove_row_from_table(rows - 1);
}

void MainWindow::del_all_points() {
	int rows = ui->coords_table_widget->rowCount();
	if (rows == 0)
		ui->answer_text_edit->setPlainText("Таблица пуста.");
	else {
		for (int i = 0; i < rows; ++i)
			ui->coords_table_widget->removeRow(0);
		ui->answer_text_edit->setPlainText("Все точки успешно удалены.");
	}
}

Triangle MainWindow::best_triangle() {
	double min_angle = 180;
	Triangle best_triangle;
	for (int i = 0; i < points.size() - 2; ++i)
		for (int j = i + 1; j < points.size() - 1; ++j)
			for (int k = j + 1; k < points.size(); ++k) {
				Triangle curr_triangle(points[i], points[j], points[k]);
				double curr_angle = curr_triangle.min_angle_hm();
				if (curr_angle < min_angle) {
					best_triangle = curr_triangle;
					min_angle = curr_angle;
				}
			}
	return best_triangle;
}

void MainWindow::show_result() {
	scene->clear();
	read_table();

	if (points.size() < 3)
		ui->answer_text_edit->setPlainText("Недостаточно точек для решения задачи.");
	else {
		Triangle triangle = best_triangle();
		std::cout << triangle.get_a().get_x() << std::endl;
		// TODO: draw(triangle);
	}
}

void MainWindow::show_task() {
	QMessageBox::information(nullptr,
							 "Условие задачи",
							 "На плоскости дано множество точек.\n"
							 "Найти треугольник с вершинами в этих точках, у которого угол, образованный высотой и медианной, "
							 "исходящими из одной вершины, минимален");
}
