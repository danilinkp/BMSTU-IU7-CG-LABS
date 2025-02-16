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
				if (curr_triangle.is_triangle()) {
					double curr_angle = curr_triangle.min_angle_hm();
//					std::cout << "area " << curr_triangle.area() << " angle " << curr_angle << std::endl;
					if (curr_angle < min_angle) {
						best_triangle = curr_triangle;
						min_angle = curr_angle;
					}
				}
			}
	return best_triangle;
}

void MainWindow::draw(const Triangle &triangle) {
	scene->clear();

	double min_x = std::min({triangle.get_a().get_x(), triangle.get_b().get_x(), triangle.get_c().get_x()});
	double max_x = std::max({triangle.get_a().get_x(), triangle.get_b().get_x(), triangle.get_c().get_x()});
	double min_y = std::min({triangle.get_a().get_y(), triangle.get_b().get_y(), triangle.get_c().get_y()});
	double max_y = std::max({triangle.get_a().get_y(), triangle.get_b().get_y(), triangle.get_c().get_y()});

	double padding = (max_x - min_x) * 0.15;
	min_x -= padding;
	max_x += padding;
	min_y -= padding;
	max_y += padding;

	double view_width = ui->graphics_view->width();
	double view_height = ui->graphics_view->height();

	double scale_x = view_width / (max_x - min_x);
	double scale_y = view_height / (max_y - min_y);
	double scale = std::min(scale_x, scale_y);

	QTransform transform;
	transform.scale(scale, -scale);
	ui->graphics_view->setTransform(transform);

	scene->setSceneRect(min_x, min_y, max_x - min_x, max_y - min_y);

	QPen triangle_pen(Qt::black, 2 / scale);
	scene->addLine(triangle.get_a().get_x(), triangle.get_a().get_y(),
				   triangle.get_b().get_x(), triangle.get_b().get_y(), triangle_pen);
	scene->addLine(triangle.get_b().get_x(), triangle.get_b().get_y(),
				   triangle.get_c().get_x(), triangle.get_c().get_y(), triangle_pen);
	scene->addLine(triangle.get_c().get_x(), triangle.get_c().get_y(),
				   triangle.get_a().get_x(), triangle.get_a().get_y(), triangle_pen);

	// Рисуем точки вершин
	double point_size = 4 / scale;
	QPen point_pen(Qt::red, point_size);
	scene->addEllipse(triangle.get_a().get_x() - point_size / 2,
					  triangle.get_a().get_y() - point_size / 2,
					  point_size, point_size, point_pen);
	scene->addEllipse(triangle.get_b().get_x() - point_size / 2,
					  triangle.get_b().get_y() - point_size / 2,
					  point_size, point_size, point_pen);
	scene->addEllipse(triangle.get_c().get_x() - point_size / 2,
					  triangle.get_c().get_y() - point_size / 2,
					  point_size, point_size, point_pen);

	QFont font;
	font.setPointSize(10);

	QTransform text_transform;
	text_transform.scale(1 / scale, -1 / scale);

	QString a_coord = QString("A(%1; %2)").arg(triangle.get_a().get_x()).arg(triangle.get_a().get_y());
	auto text_a = scene->addText(a_coord, font);
	text_a->setTransform(text_transform);
	text_a->setPos(triangle.get_a().get_x(), triangle.get_a().get_y());

	QString b_coord = QString("B(%1; %2)").arg(triangle.get_b().get_x()).arg(triangle.get_b().get_y());
	auto text_b = scene->addText(b_coord, font);
	text_b->setTransform(text_transform);
	text_b->setPos(triangle.get_b().get_x(), triangle.get_b().get_y());

	QString c_coord = QString("C(%1; %2)").arg(triangle.get_c().get_x()).arg(triangle.get_c().get_y());
	auto text_c = scene->addText(c_coord, font);
	text_c->setTransform(text_transform);
	text_c->setPos(triangle.get_c().get_x(), triangle.get_c().get_y());

	Point vertex, opposite1, opposite2;
	triangle.get_points_for_min_angle(vertex, opposite1, opposite2);

	Point M((opposite1.get_x() + opposite2.get_x()) / 2,
			(opposite1.get_y() + opposite2.get_y()) / 2);

	double a = std::sqrt(std::pow(opposite2.get_x() - opposite1.get_x(), 2) +
		std::pow(opposite2.get_y() - opposite1.get_y(), 2));
	double b = std::sqrt(std::pow(vertex.get_x() - opposite1.get_x(), 2) +
		std::pow(vertex.get_y() - opposite1.get_y(), 2));
	double c = std::sqrt(std::pow(vertex.get_x() - opposite2.get_x(), 2) +
		std::pow(vertex.get_y() - opposite2.get_y(), 2));

	double x = (b * b - c * c) / (2 * a) + a / 2;
	double t = x / a;
	Point H(opposite1.get_x() + t * (opposite2.get_x() - opposite1.get_x()),
			opposite1.get_y() + t * (opposite2.get_y() - opposite1.get_y()));

	QPen median_pen(Qt::green, 2 / scale);
	scene->addLine(vertex.get_x(), vertex.get_y(), M.get_x(), M.get_y(), median_pen);

	QPen height_pen(Qt::red, 2 / scale);
	scene->addLine(vertex.get_x(), vertex.get_y(), H.get_x(), H.get_y(), height_pen);

	scene->addEllipse(M.get_x() - point_size / 2, M.get_y() - point_size / 2,
					  point_size, point_size, point_pen);
	scene->addEllipse(H.get_x() - point_size / 2, H.get_y() - point_size / 2,
					  point_size, point_size, point_pen);

	QString m_coord = QString("M(%1; %2)").arg(M.get_x()).arg(M.get_y());
	auto text_m = scene->addText(m_coord, font);
	text_m->setTransform(text_transform);
	text_m->setPos(M.get_x(), M.get_y());

	QString h_coord = QString("H(%1; %2)").arg(H.get_x()).arg(H.get_y());
	auto text_h = scene->addText(h_coord, font);
	text_h->setTransform(text_transform);
	text_h->setPos(H.get_x(), H.get_y());
}

void MainWindow::show_result() {
	scene->clear();
	read_table();

	if (points.size() < 3)
		ui->answer_text_edit->setPlainText("Недостаточно точек для решения задачи.");
	else {
		Triangle triangle = best_triangle();
		if (!triangle.is_triangle())
			ui->answer_text_edit->setPlainText("На заданном множестве точек нет треугольника.");
		else {
			draw(triangle);
			QString result = QString("Найден треугольник с вершинами:\n"
									 "A(%1; %2)\n"
									 "B(%3; %4)\n"
									 "C(%5; %6)\n"
									 "Минимальный угол между высотой и медианой: %7°")
				.arg(triangle.get_a().get_x())
				.arg(triangle.get_a().get_y())
				.arg(triangle.get_b().get_x())
				.arg(triangle.get_b().get_y())
				.arg(triangle.get_c().get_x())
				.arg(triangle.get_c().get_y())
				.arg(triangle.min_angle_hm());

			ui->answer_text_edit->setPlainText(result);
		}
	}
}

void MainWindow::show_task() {
	QMessageBox::information(nullptr,
							 "Условие задачи",
							 "На плоскости дано множество точек.\n"
							 "Найти треугольник с вершинами в этих точках, у которого угол, образованный высотой и медианной, "
							 "исходящими из одной вершины, минимален");
}
