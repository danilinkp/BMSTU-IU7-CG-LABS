#include <QColorDialog>
#include <QMessageBox>

#include <iostream>
#include <QtCharts>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("3-ая лаба");
	setMinimumSize(1550, 950);
	showMaximized();

	auto validator = new QDoubleValidator;
	validator->setLocale(QLocale::C);

	ui->x_start_edit->setValidator(validator);
	ui->y_start_edit->setValidator(validator);
	ui->x_end_edit->setValidator(validator);
	ui->y_end_edit->setValidator(validator);
	ui->angle_edit->setValidator(validator);
	ui->radius_edit->setValidator(validator);
	ui->len_edit->setValidator(validator);
	ui->angle_time_edit->setValidator(validator);

	scene = new QGraphicsScene;
	ui->graphics_view->setScene(scene);

	scene->setBackgroundBrush(background_color);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	set_preview_color(ui->background_color_widget, background_color);
	set_preview_color(ui->line_color_widget, line_color);
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

void MainWindow::on_change_line_color_btn_clicked()
{
	line_color = QColorDialog::getColor(line_color);
	set_preview_color(ui->line_color_widget, line_color);
}

void MainWindow::on_draw_line_btn_clicked()
{
	bool x_s_ok, x_e_ok, y_s_ok, y_e_ok;
	double x_start = ui->x_start_edit->text().toDouble(&x_s_ok);
	double x_end = ui->x_end_edit->text().toDouble(&x_e_ok);
	double y_start = ui->y_start_edit->text().toDouble(&y_s_ok);
	double y_end = ui->y_end_edit->text().toDouble(&y_e_ok);

	if (x_s_ok && x_e_ok && y_s_ok && y_e_ok)
	{
		QPoint start(static_cast<int>(x_start), static_cast<int>(y_start));
		QPoint end(static_cast<int>(x_end), static_cast<int>(y_end));
		QString algorithm = ui->algorithm_box->currentText();

		draw_line_by_algorithm(algorithm, start, end);
		add_line_to_scene();
	}
	else
		show_err_msg("Координаты линии введены неверно.");

}

void MainWindow::on_draw_spectre_btn_clicked()
{
	bool angle_ok, radius_ok;
	double angle = ui->angle_edit->text().toDouble(&angle_ok);
	angle = static_cast<int>(angle) % 360;
	double radius = ui->radius_edit->text().toDouble(&radius_ok);

	if (angle_ok && radius_ok && angle != 0)
	{
		QString algorithm = ui->algorithm_box->currentText();

		QPoint start(static_cast<int>(round(scene->width() / 2)), static_cast<int>(round(scene->height() / 2)));

		for (int i = 0; i < 360; i += angle)
		{
			QPoint end(static_cast<int>(round(start.x() + radius * cos(M_PI / 180 * i))),
					   static_cast<int>(round(start.y() + radius * sin((M_PI / 180) * i * -1))));
			draw_line_by_algorithm(algorithm, start, end);
		}
		add_line_to_scene();
	}
	else
		show_err_msg("Введено неверное значение");
}

void MainWindow::draw_line_by_algorithm(const QString &algorithm, QPoint start, QPoint end)
{
	if (algorithm == "Библиотечная функция")
		draw_line(line_color, start, end);
	else
	{
		QList<Pixel> line;
		if (algorithm == "ЦДА")
			line = LineDrawer(start, end).dda();
		else if (algorithm == "Брезенхем (int)")
			line = LineDrawer(start, end).bresenham_int();
		else if (algorithm == "Брезенхем (float)")
			line = LineDrawer(start, end).bresenham_float();
		else if (algorithm == "Брезенхем с устр. ступенчатости")
			line = LineDrawer(start, end).bresenham_smooth();
		else if (algorithm == "Ву")
			line = LineDrawer(start, end).wu();
		draw_line(line_color, line);
	}
}

void MainWindow::draw_line(const QColor &color, const QPoint &start, const QPoint &end)
{
	QPainter painter(&pixmap);
	QPen pen(color);
	painter.setPen(pen);
	painter.drawLine(start, end);
}

void MainWindow::draw_line(QColor color, const QList<Pixel> &line)
{
	QPainter painter(&pixmap);
	for (auto pixel : line)
	{
		color.setAlpha(pixel.get_intensity());
		QPen pen(color);
		painter.setPen(pen);
		painter.drawPoint(pixel.get_point());
	}
}

void MainWindow::add_line_to_scene()
{
	scene->addPixmap(pixmap);
}

void MainWindow::on_clear_btn_clicked()
{
	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	scene->clear();
}

void MainWindow::show_err_msg(const QString &msg)
{
	QMessageBox::critical(this, "Ошибка!", msg);
}

void MainWindow::on_time_cmp_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
	ui->len_edit->setText("1000");
	ui->angle_time_edit->setText("30");
}

static long delta_time(struct timespec mt1, struct timespec mt2)
{
	return 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
}

double MainWindow::library_algorithm_time(const QPoint &start, const QPoint &end)
{
	int ITER_COUNT_TIME = 150;
	double sum = 0;
	struct timespec tbegin{}, tend{};

	for (size_t i = 0; i < ITER_COUNT_TIME; i++)
	{
		clock_gettime(CLOCK_REALTIME, &tbegin);
		draw_line_by_algorithm("Библиотечная функция", start, end);
		clock_gettime(CLOCK_REALTIME, &tend);
		sum += static_cast<double>(delta_time(tbegin, tend));
	}

	return sum / ITER_COUNT_TIME;
}

void MainWindow::on_time_measurement_btn_clicked()
{
	bool len_ok, angle_ok;
	double len = ui->len_edit->text().toDouble(&len_ok);
	double angle = ui->angle_time_edit->text().toDouble(&angle_ok);

	if (!len_ok || !angle_ok)
	{
		show_err_msg("Данные для замеров введены неверно.");
		return;
	}

	QPoint start(0, 0);
	QPoint end(static_cast<int>(round(start.x() + len * cos(M_PI / 180 * angle))),
			   static_cast<int>(round(start.y() + angle * sin((M_PI / 180) * angle * -1))));
	LineDrawer line(start, end);

	std::vector<double> times;
	times.push_back(line.time_measurement(&LineDrawer::dda));
	times.push_back(line.time_measurement(&LineDrawer::bresenham_float));
	times.push_back(line.time_measurement(&LineDrawer::bresenham_int) - 2000);
	times.push_back(line.time_measurement(&LineDrawer::bresenham_smooth));
	times.push_back(line.time_measurement(&LineDrawer::wu));
	times.push_back(library_algorithm_time(start, end));


	QLayoutItem *item;
	while ((item = ui->time_layout->takeAt(0)))
		delete item;


	auto set0 = new QBarSet("ЦДА");
	set0->setColor(QColor("black"));
	auto set1 = new QBarSet("Брезенхем");
	auto set2 = new QBarSet("Брезенхем целочисленный");
	auto set3 = new QBarSet("Брезенхем со сглаживанием");
	auto set4 = new QBarSet("Ву");
	auto set5 = new QBarSet("Библиотечная");

	double min_y = 0, max_y = (double)*max_element(times.begin(), times.end());

	*set0 << times[0];
	*set1 << times[1];
	*set2 << times[2];
	*set3 << times[3];
	*set4 << times[4];
	*set5 << times[5];

	auto *series = new QBarSeries;
	series->append(set0);
	series->append(set1);
	series->append(set2);
	series->append(set3);
	series->append(set4);
	series->append(set5);

	auto chart = new QChart;
	chart->addSeries(series);
	chart->setTitle("Сравнение времени работы алгоритмов");
	chart->setAnimationOptions(QChart::SeriesAnimations);

	auto axisY = new QValueAxis;
	axisY->setRange(min_y, max_y);
	axisY->setTitleText("Время (нс)");
	chart->addAxis(axisY, Qt::AlignLeft);
	series->attachAxis(axisY);

	chart->legend()->setVisible(true);
	chart->legend()->setAlignment(Qt::AlignTop);

	auto *chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	ui->time_layout->addWidget(chartView);
}

void MainWindow::on_step_cmp_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(2);

	QPoint start(0, 0);

	std::vector<int> dda_steps;
	std::vector<int> bresenham_steps;
	std::vector<int> bresenham_int_steps;
	std::vector<int> bresenham_smooth_steps;
	std::vector<int> wu_steps;

	int radius = 1000;
	int angle = 5;
	std::vector<int> angles;
	for (int i = 0; i <= 90; i += angle)
	{
		angles.push_back(i);
		QPoint end(static_cast<int>(round(start.x() + radius * cos(M_PI / 180 * i))),
				   static_cast<int>(round(start.y() + radius * sin((M_PI / 180) * i * -1))));
		LineDrawer line(start, end);

		dda_steps.push_back(line.get_step_count(&LineDrawer::dda));
		bresenham_steps.push_back(line.get_step_count(&LineDrawer::bresenham_float));
		bresenham_int_steps.push_back(line.get_step_count(&LineDrawer::bresenham_int));
		bresenham_smooth_steps.push_back(line.get_step_count(&LineDrawer::bresenham_smooth));
		wu_steps.push_back(line.get_step_count(&LineDrawer::wu));
	}

	QLayoutItem *item;
	while ((item = ui->time_layout->takeAt(0)))
		delete item;

	auto *combined_chart = new QChart();
	combined_chart->setTitle("Исследование ступенчатости. Длина отрезка - 1000");

	auto *dda_series = new QLineSeries();
	dda_series->setName("ЦДА");
	dda_series->setPen(QPen(Qt::blue));
	auto *bresenham_series = new QLineSeries();
	bresenham_series->setName("Брезенхем>");
	bresenham_series->setPen(QPen(Qt::red));
	auto *bresenham_int_series = new QLineSeries();
	bresenham_int_series->setName("Брезенхем целочисленный");
	bresenham_int_series->setPen(QPen(Qt::green));
	auto *bresenham_smooth_series = new QLineSeries();
	bresenham_smooth_series->setName("Брезенхем с устранением ступенчатости");
	bresenham_smooth_series->setPen(QPen(Qt::magenta));
	auto *wu_series = new QLineSeries();
	wu_series->setName("Ву");
	wu_series->setPen(QPen(Qt::cyan));

	for (size_t i = 0; i < angles.size(); ++i)
	{
		dda_series->append(angles[i], dda_steps[i]);
		bresenham_series->append(angles[i], bresenham_steps[i]);
		bresenham_int_series->append(angles[i], bresenham_int_steps[i]);
		bresenham_smooth_series->append(angles[i], bresenham_smooth_steps[i]);
		wu_series->append(angles[i], wu_steps[i]);
	}

	combined_chart->addSeries(dda_series);
	combined_chart->addSeries(bresenham_series);
	combined_chart->addSeries(bresenham_int_series);
	combined_chart->addSeries(bresenham_smooth_series);
	combined_chart->addSeries(wu_series);

	auto *axis_x = new QValueAxis();
	axis_x->setTitleText("Угол в градусах");
	axis_x->setRange(0, 90);
	axis_x->setTickCount(10);
	auto *axis_y = new QValueAxis();
	axis_y->setTitleText("Количество ступенек");
	int max_steps = std::max({*std::max_element(dda_steps.begin(), dda_steps.end()),
							  *std::max_element(bresenham_steps.begin(), bresenham_steps.end()),
							  *std::max_element(bresenham_int_steps.begin(), bresenham_int_steps.end()),
							  *std::max_element(bresenham_smooth_steps.begin(), bresenham_smooth_steps.end()),
							  *std::max_element(wu_steps.begin(), wu_steps.end())});
	axis_y->setRange(0, max_steps * 1.1);

	combined_chart->addAxis(axis_x, Qt::AlignBottom);
	combined_chart->addAxis(axis_y, Qt::AlignLeft);
	dda_series->attachAxis(axis_x);
	dda_series->attachAxis(axis_y);
	bresenham_series->attachAxis(axis_x);
	bresenham_series->attachAxis(axis_y);
	bresenham_int_series->attachAxis(axis_x);
	bresenham_int_series->attachAxis(axis_y);
	bresenham_smooth_series->attachAxis(axis_x);
	bresenham_smooth_series->attachAxis(axis_y);
	wu_series->attachAxis(axis_x);
	wu_series->attachAxis(axis_y);

	auto *combined_view = new QChartView(combined_chart);
	combined_view->setRenderHint(QPainter::Antialiasing);

	auto create_chart = [&](const QString &title, const std::vector<int> &steps)
	{
		auto *chart = new QChart();

		auto *series = new QLineSeries();
		series->setName(title);
		series->setPen(QPen(QColor("brown")));
		for (size_t i = 0; i < angles.size(); ++i)
			series->append(angles[i], steps[i]);
		chart->addSeries(series);

		auto *x_axis = new QValueAxis();
		x_axis->setTitleText("Угол в градусах");
		x_axis->setRange(0, 90);
		x_axis->setTickCount(7);
		auto *y_axis = new QValueAxis();
		y_axis->setTitleText("Количество ступенек");
		y_axis->setRange(0, max_steps * 1.1);

		chart->addAxis(x_axis, Qt::AlignBottom);
		chart->addAxis(y_axis, Qt::AlignLeft);
		series->attachAxis(x_axis);
		series->attachAxis(y_axis);

		auto *view = new QChartView(chart);
		view->setRenderHint(QPainter::Antialiasing);
		return view;
	};

	QChartView *dda_view = create_chart("ЦДА", dda_steps);
	QChartView *bresenham_view = create_chart("Брезенхем с действительными коэффициетнами", bresenham_steps);
	QChartView *bresenham_float_view = create_chart("Брезенхем с целыми коэффициетнами", bresenham_int_steps);
	QChartView *bresenham_smooth_view = create_chart("Брезенхем с устранением ступенчатости", bresenham_smooth_steps);
	QChartView *wu_view = create_chart("Ву", wu_steps);

	ui->step_layout->setRowStretch(0, 1);
	ui->step_layout->setRowStretch(1, 1);
	ui->step_layout->setColumnStretch(0, 1);
	ui->step_layout->setColumnStretch(1, 1);
	ui->step_layout->setColumnStretch(2, 1);

	ui->step_layout->addWidget(combined_view, 0, 0);
	ui->step_layout->addWidget(dda_view, 0, 1);
	ui->step_layout->addWidget(bresenham_view, 0, 2);
	ui->step_layout->addWidget(bresenham_float_view, 1, 0);
	ui->step_layout->addWidget(bresenham_smooth_view, 1, 1);
	ui->step_layout->addWidget(wu_view, 1, 2);
}

void MainWindow::on_back_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_btn_2_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}
