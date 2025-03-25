#include <QColorDialog>
#include <QMessageBox>
#include <iostream>
#include <QtCharts>
#include <ranges>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("4-ая лаба");
	showMaximized();

	auto validator = new QDoubleValidator;
	validator->setLocale(QLocale::C);

	ui->x_centre_edit->setValidator(validator);
	ui->y_centre_edit->setValidator(validator);
	ui->radius_edit->setValidator(validator);
	ui->rx_edit->setValidator(validator);
	ui->ry_edit->setValidator(validator);
	ui->x_spectra_edit->setValidator(validator);
	ui->y_spectra_edit->setValidator(validator);
	ui->step_spectra_edit->setValidator(validator);
	ui->num_spectra_edit->setValidator(validator);
	ui->start_radius_edit->setValidator(validator);
	ui->start_w_edit->setValidator(validator);
	ui->start_h_edit->setValidator(validator);

	scene = new QGraphicsScene;
	ui->graphics_view->setScene(scene);

	scene->setBackgroundBrush(background_color);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	set_preview_color(ui->background_color_widget, background_color);
	set_preview_color(ui->line_color_widget, figure_color);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete scene;
}

void MainWindow::set_preview_color(QWidget *widget, const QColor &color)
{
	QPalette palette;
	palette.setColor(QPalette::Window, color);
	widget->setAutoFillBackground(true);
	widget->setPalette(palette);
}

void MainWindow::on_change_back_color_btn_clicked()
{
	background_color = QColorDialog::getColor(background_color);
	set_preview_color(ui->background_color_widget, background_color);

	scene->setBackgroundBrush(background_color);
}

void MainWindow::on_change_figure_color_btn_clicked()
{
	figure_color = QColorDialog::getColor(figure_color);
	set_preview_color(ui->line_color_widget, figure_color);
}

void MainWindow::on_draw_circle_btn_clicked()
{
	bool x_ok, y_ok, radius_ok;
	int x_center = static_cast<int>(ui->x_centre_edit->text().toDouble(&x_ok));
	int y_center = static_cast<int>(ui->y_centre_edit->text().toDouble(&y_ok));
	int radius = static_cast<int>(ui->radius_edit->text().toDouble(&radius_ok));

	if (x_ok && y_ok && radius_ok && radius > 0)
	{
		QString algorithm = ui->algorithm_box->currentText();
		QPoint center(x_center, y_center);

		draw_circle_by_algorithm(algorithm, center, radius);
	}
	else
		show_err_msg("Радиус должен быть положительным");
}

void MainWindow::draw_circle_by_algorithm(const QString &algorithm, const QPoint &center, int radius)
{
	if (algorithm == "Библиотечная функция")
		draw_figure(figure_color, center, radius);
	else
	{
		CircleDrawer circle(center, radius);
		QList<QPoint> points;
		if (algorithm == "Каноническое уравнение")
			points = circle.canonical();
		else if (algorithm == "Параметрическое уравнение")
			points = circle.parametric();
		else if (algorithm == "Брезенхем")
			points = circle.bresenham();
		else if (algorithm == "Средней точки")
			points = circle.middle_point();

		draw_figure(figure_color, points);
	}
	add_figure_to_scene();
}

void MainWindow::on_draw_ellipse_btn_clicked()
{
	bool x_ok, y_ok, rx_ok, ry_ok;
	int x_center = static_cast<int>(ui->x_centre_edit->text().toDouble(&x_ok));
	int y_center = static_cast<int>(ui->y_centre_edit->text().toDouble(&y_ok));
	int rx = static_cast<int>(ui->rx_edit->text().toDouble(&rx_ok));
	int ry = static_cast<int>(ui->ry_edit->text().toDouble(&ry_ok));

	if (x_ok && y_ok && rx_ok && ry_ok && rx > 0 && ry > 0)
	{
		QString algorithm = ui->algorithm_box->currentText();
		QPoint center(x_center, y_center);

		draw_ellipse_by_algorithm(algorithm, center, rx, ry);
	}
	else
		show_err_msg("Значения полуосей должны быть положительными");
}

void MainWindow::draw_ellipse_by_algorithm(const QString &algorithm, const QPoint &center, int rx, int ry)
{
	if (algorithm == "Библиотечная функция")
		draw_figure(figure_color, center, rx, ry);
	else
	{
		EllipseDrawer ellipse(center, rx, ry);
		QList<QPoint> points;
		if (algorithm == "Каноническое уравнение")
			points = ellipse.canonical();
		else if (algorithm == "Параметрическое уравнение")
			points = ellipse.parametric();
		else if (algorithm == "Брезенхем")
			points = ellipse.bresenham();
		else if (algorithm == "Средней точки")
			points = ellipse.middle_point();
		draw_figure(figure_color, points);
	}
	add_figure_to_scene();
}

void MainWindow::draw_figure(const QColor &color, const QPoint &center, int radius)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	painter.drawEllipse(center, radius, radius);
}

void MainWindow::draw_figure(const QColor &color, const QPoint &center, int rx, int ry)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	painter.drawEllipse(center, rx, ry);
}

void MainWindow::draw_figure(const QColor &color, const QList<QPoint> &figure)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	for (auto pixel : figure)
		painter.drawPoint(pixel);
}

void MainWindow::on_draw_circle_spectra_btn_clicked()
{
	bool x_ok, y_ok, step_ok, num_ok, radius_ok;
	int x_center = static_cast<int>(ui->x_spectra_edit->text().toDouble(&x_ok));
	int y_center = static_cast<int>(ui->y_spectra_edit->text().toDouble(&y_ok));
	int step = static_cast<int>(ui->step_spectra_edit->text().toDouble(&step_ok));
	int num = static_cast<int>(ui->num_spectra_edit->text().toDouble(&num_ok));
	int start_radius = static_cast<int>(ui->start_radius_edit->text().toDouble(&radius_ok));

	if (x_ok && y_ok && step_ok && num_ok && radius_ok)
	{
		QString algorithm = ui->algorithm_box->currentText();
		QPoint center(x_center, y_center);
		for (int i = 0; i < num; i++)
			draw_circle_by_algorithm(algorithm, center, start_radius + i * step);
	}
	else
		show_err_msg("Ошибка ввода параметров");
}

void MainWindow::on_draw_ellipse_spectra_btn_clicked()
{
	bool x_ok, y_ok, step_ok, num_ok, rx_ok, ry_ok;
	int x_center = static_cast<int>(ui->x_spectra_edit->text().toDouble(&x_ok));
	int y_center = static_cast<int>(ui->y_spectra_edit->text().toDouble(&y_ok));
	int step = static_cast<int>(ui->step_spectra_edit->text().toDouble(&step_ok));
	int num = static_cast<int>(ui->num_spectra_edit->text().toDouble(&num_ok));
	int start_rx = static_cast<int>(ui->start_w_edit->text().toDouble(&rx_ok));
	int start_ry = static_cast<int>(ui->start_h_edit->text().toDouble(&ry_ok));

	if (x_ok && y_ok && step_ok && num_ok && rx_ok && ry_ok)
	{
		QString algorithm = ui->algorithm_box->currentText();
		QPoint center(x_center, y_center);
		for (int i = 0; i < num; i++)
			draw_ellipse_by_algorithm(algorithm, center, start_rx + i * step, start_ry + i * step);
	}
	else
		show_err_msg("Ошибка ввода параметров");
}

void MainWindow::add_figure_to_scene()
{
	scene->addPixmap(pixmap);
}

void MainWindow::show_err_msg(const QString &msg)
{
	QMessageBox::critical(this, "Ошибка!", msg);
}

void MainWindow::on_clear_btn_clicked()
{
	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	scene->clear();
}

double MainWindow::lib_time_measurement(const QPoint &center, int radius)
{
	int ITER_COUNT_TIME = 50;
	double sum = 0;

	for (size_t i = 0; i < ITER_COUNT_TIME; i++)
	{
		auto start = std::chrono::high_resolution_clock::now();
		draw_figure(figure_color, center, radius);
		auto end = std::chrono::high_resolution_clock::now();
		sum += std::chrono::duration<double, std::nano>(end - start).count();
	}
	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	scene->clear();
	return sum / ITER_COUNT_TIME;
}

double MainWindow::lib_time_measurement(const QPoint &center, int rx, int ry)
{
	int ITER_COUNT_TIME = 50;
	double sum = 0;

	for (size_t i = 0; i < ITER_COUNT_TIME; i++)
	{
		auto start = std::chrono::high_resolution_clock::now();
		draw_figure(figure_color, center, rx, ry);
		auto end = std::chrono::high_resolution_clock::now();
		sum += std::chrono::duration<double, std::nano>(end - start).count();
	}
	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	scene->clear();
	return sum / ITER_COUNT_TIME;
}

void MainWindow::on_circle_time_cmp_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);

	auto sequence = std::ranges::iota_view(1000, 10001) | std::views::stride(1000);
	std::vector<int> radii = sequence | std::ranges::to<std::vector>();

	QPoint center(500, 500);

	std::vector<double> times_canonical;
	std::vector<double> times_parametric;
	std::vector<double> times_bresenham;
	std::vector<double> times_middle_point;
	std::vector<double> times_lib;

	for (auto radius : radii)
	{
		CircleDrawer circle(center, radius);
		times_canonical.push_back(circle.time_measurement(&CircleDrawer::canonical));
		times_parametric.push_back(circle.time_measurement(&CircleDrawer::parametric));
		times_bresenham.push_back(circle.time_measurement(&CircleDrawer::bresenham));
		times_middle_point.push_back(circle.time_measurement(&CircleDrawer::middle_point));
		times_lib.push_back(lib_time_measurement(center, radius));
	}


	QLayoutItem * item;
	while ((item = ui->time_layout->takeAt(0)))
		delete item;

	auto *result_chart = new QChart();
	QFont title_font = result_chart->titleFont();
	title_font.setPointSize(16);
	title_font.setBold(true);
	result_chart->setTitleFont(title_font);
	result_chart->setTitle("Замеры времени для построения окружности");
	result_chart->setAnimationOptions(QChart::SeriesAnimations);

	QFont legend_font = result_chart->legend()->font();
	legend_font.setPointSize(14);
	result_chart->legend()->setFont(legend_font);

	auto *canonical_series = new QLineSeries();
	canonical_series->setName("Каноническое уравнение");
	auto *parametric_series = new QLineSeries();
	parametric_series->setName("Параметрическое уравнение");
	auto *bresenham_series = new QLineSeries();
	bresenham_series->setName("Алгоритм Брезенхема");
	auto *mid_point_series = new QLineSeries();
	mid_point_series->setName("Алгоритм средней точки");
	auto *lib_series = new QLineSeries();
	lib_series->setName("Библиотечная функция");

	for (size_t i = 0; i < radii.size(); i++)
	{
		canonical_series->append(radii[i], times_canonical[i]);
		parametric_series->append(radii[i], times_parametric[i]);
		bresenham_series->append(radii[i], times_bresenham[i]);
		mid_point_series->append(radii[i], times_middle_point[i]);
		lib_series->append(radii[i], times_lib[i]);
	}

	result_chart->addSeries(canonical_series);
	result_chart->addSeries(parametric_series);
	result_chart->addSeries(bresenham_series);
	result_chart->addSeries(mid_point_series);
	result_chart->addSeries(lib_series);

	auto *axis_x = new QValueAxis();
	axis_x->setTitleText("Радиус");
	axis_x->setRange(1000, 10000);
	axis_x->setTickCount(10);

	auto *axis_y = new QValueAxis();
	axis_y->setTitleText("Время");
	double max_steps = std::max({*std::max_element(times_canonical.begin(), times_canonical.end()),
							  *std::max_element(times_parametric.begin(), times_parametric.end()),
							  *std::max_element(times_bresenham.begin(), times_bresenham.end()),
							  *std::max_element(times_middle_point.begin(), times_middle_point.end()),
							  *std::max_element(times_lib.begin(), times_lib.end())});
	axis_y->setRange(0, max_steps * 1.1);

	result_chart->addAxis(axis_x, Qt::AlignBottom);
	result_chart->addAxis(axis_y, Qt::AlignLeft);
	canonical_series->attachAxis(axis_x);
	canonical_series->attachAxis(axis_y);
	bresenham_series->attachAxis(axis_x);
	bresenham_series->attachAxis(axis_y);
	parametric_series->attachAxis(axis_x);
	parametric_series->attachAxis(axis_y);
	mid_point_series->attachAxis(axis_x);
	mid_point_series->attachAxis(axis_y);
	lib_series->attachAxis(axis_x);
	lib_series->attachAxis(axis_y);

	auto *result_view = new QChartView(result_chart);
	result_view->setRenderHint(QPainter::Antialiasing);

	ui->time_layout->addWidget(result_view);
}

void MainWindow::on_ellipse_time_cmp_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(2);

	std::vector<std::pair<int, int>> radii;
	for (int i = 1000; i <= 10000; i += 1000)
	{
		std::pair<int, int> semi_axis(i + 100, i);
		radii.push_back(semi_axis);
	}

	QPoint center(500, 500);

	std::vector<double> times_canonical;
	std::vector<double> times_parametric;
	std::vector<double> times_bresenham;
	std::vector<double> times_middle_point;
	std::vector<double> times_lib;

	for (auto radius : radii)
	{
		EllipseDrawer ellipse(center, radius.first, radius.second);
		times_canonical.push_back(ellipse.time_measurement(&EllipseDrawer::canonical));
		times_parametric.push_back(ellipse.time_measurement(&EllipseDrawer::parametric));
		times_bresenham.push_back(ellipse.time_measurement(&EllipseDrawer::bresenham));
		times_middle_point.push_back(ellipse.time_measurement(&EllipseDrawer::middle_point));
		times_lib.push_back(lib_time_measurement(center, radius.first, radius.second));
	}


	QLayoutItem * item;
	while ((item = ui->time_layout_2->takeAt(0)))
		delete item;

	auto *result_chart = new QChart();
	result_chart->setTitle("Замеры времени для построения эллипса");
	result_chart->setAnimationOptions(QChart::SeriesAnimations);

	QFont title_font = result_chart->titleFont();
	title_font.setPointSize(16);
	title_font.setBold(true);
	result_chart->setTitleFont(title_font);
	QFont legend_font = result_chart->legend()->font();
	legend_font.setPointSize(14);
	result_chart->legend()->setFont(legend_font);

	auto *canonical_series = new QLineSeries();
	canonical_series->setName("Каноническое уравнение");
	auto *parametric_series = new QLineSeries();
	parametric_series->setName("Параметрическое уравнение");
	auto *bresenham_series = new QLineSeries();
	bresenham_series->setName("Алгоритм Брезенхема");
	auto *mid_point_series = new QLineSeries();
	mid_point_series->setName("Алгоритм средней точки");
	auto *lib_series = new QLineSeries();
	lib_series->setName("Библиотечная функция");

	for (size_t i = 0; i < radii.size(); i++)
	{
		canonical_series->append(radii[i].second, times_canonical[i]);
		parametric_series->append(radii[i].second, times_parametric[i]);
		bresenham_series->append(radii[i].second, times_bresenham[i]);
		mid_point_series->append(radii[i].second, times_middle_point[i]);
		lib_series->append(radii[i].second, times_lib[i]);
	}

	result_chart->addSeries(canonical_series);
	result_chart->addSeries(parametric_series);
	result_chart->addSeries(bresenham_series);
	result_chart->addSeries(mid_point_series);
	result_chart->addSeries(lib_series);

	auto *axis_x = new QValueAxis();
	axis_x->setTitleText("Радиус");
	axis_x->setRange(1000, 10000);
	axis_x->setTickCount(10);

	auto *axis_y = new QValueAxis();
	axis_y->setTitleText("Время");
	double max_steps = std::max({*std::max_element(times_canonical.begin(), times_canonical.end()),
								 *std::max_element(times_parametric.begin(), times_parametric.end()),
								 *std::max_element(times_bresenham.begin(), times_bresenham.end()),
								 *std::max_element(times_middle_point.begin(), times_middle_point.end()),
								 *std::max_element(times_lib.begin(), times_lib.end())});
	axis_y->setRange(0, max_steps * 1.1);

	result_chart->addAxis(axis_x, Qt::AlignBottom);
	result_chart->addAxis(axis_y, Qt::AlignLeft);
	canonical_series->attachAxis(axis_x);
	canonical_series->attachAxis(axis_y);
	bresenham_series->attachAxis(axis_x);
	bresenham_series->attachAxis(axis_y);
	parametric_series->attachAxis(axis_x);
	parametric_series->attachAxis(axis_y);
	mid_point_series->attachAxis(axis_x);
	mid_point_series->attachAxis(axis_y);
	lib_series->attachAxis(axis_x);
	lib_series->attachAxis(axis_y);

	auto *result_view = new QChartView(result_chart);
	result_view->setRenderHint(QPainter::Antialiasing);

	ui->time_layout_2->addWidget(result_view);
}

void MainWindow::on_back_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_btn_2_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}