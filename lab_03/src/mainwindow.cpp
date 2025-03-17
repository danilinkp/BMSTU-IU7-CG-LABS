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

	scene = new QGraphicsScene;
	ui->graphics_view->setScene(scene);

	scene->setBackgroundBrush(background_color);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);

	qDebug() << "Graphics view size in constructor:" << ui->graphics_view->size();

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
	double radius = ui->radius_edit->text().toDouble(&radius_ok);

	if (angle_ok && radius_ok)
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
	bool x_s_ok, x_e_ok, y_s_ok, y_e_ok;
	double x_start = ui->x_start_edit->text().toDouble(&x_s_ok);
	double x_end = ui->x_end_edit->text().toDouble(&x_e_ok);
	double y_start = ui->y_start_edit->text().toDouble(&y_s_ok);
	double y_end = ui->y_end_edit->text().toDouble(&y_e_ok);

	if (!x_s_ok || !x_e_ok || !y_s_ok || !y_e_ok)
	{
		show_err_msg("Координата введена неверно.");
		return;
	}

	QPoint start(x_start, y_start);
	QPoint end(x_end, y_end);
	LineDrawer line(start, end);

	std::vector<double> times;
	times.push_back(line.time_measurement(&LineDrawer::dda));
	times.push_back(line.time_measurement(&LineDrawer::bresenham_float));
	times.push_back(line.time_measurement(&LineDrawer::bresenham_int));
	times.push_back(line.time_measurement(&LineDrawer::bresenham_smooth));
	times.push_back(line.time_measurement(&LineDrawer::wu));

	ui->stackedWidget->setCurrentIndex(1);

	QLayoutItem *item;
	while ((item = ui->time_layout->takeAt(0)))
		delete item;


	auto set0 = new QBarSet("ЦДА");
	auto set1 = new QBarSet("Брезенхем");
	auto set2 = new QBarSet("Брезенхем целочисленный");
	auto set3 = new QBarSet("Брезенхем со сглаживанием");
	auto set4 = new QBarSet("Ву");


	double min_y = 0, max_y = (double) *max_element(times.begin(), times.end());

	*set0 << times[0];
	*set1 << times[1];
	*set2 << times[2];
	*set3 << times[3];
	*set4 << times[4];

	QBarSeries *series = new QBarSeries;
	series->append(set0);
	series->append(set1);
	series->append(set2);
	series->append(set3);
	series->append(set4);

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
	chart->legend()->setAlignment(Qt::AlignBottom);

	auto *chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	ui->time_layout->addWidget(chartView);
}

void MainWindow::on_back_btn_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}
