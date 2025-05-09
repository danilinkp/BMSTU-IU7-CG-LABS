#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDoubleValidator>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("Polygon Filling with Holes");

	// Setup validator
	auto *validator = new QDoubleValidator(this);
	validator->setLocale(QLocale::C);
	ui->x_coord_edit->setValidator(validator);
	ui->y_coord_edit->setValidator(validator);

	ui->points_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->label_4->setStyleSheet("font-size: 16px;");

	scene = new QGraphicsScene(this);
	ui->graphics_view->setScene(scene);
	scene->setBackgroundBrush(background_color);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);
	filled_pixmap = pixmap;
	scene->addPixmap(pixmap);

	set_preview_color(ui->background_color_widget, background_color);
	set_preview_color(ui->line_color_widget, figure_color);

	ui->without_delay_radio_btn->setChecked(true);
	delay_mode = false;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::set_preview_color(QWidget *widget, const QColor &color)
{
	QPalette palette = widget->palette();
	palette.setColor(QPalette::Window, color);
	widget->setPalette(palette);
	widget->setAutoFillBackground(true);
}

void MainWindow::draw_polygon()
{
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen("black");
	QList<Edge> edges = polygon.create_edges();
	for (const Edge &edge : edges)
		painter.drawLine(edge.start, edge.end);

	scene->clear();
	scene->addPixmap(pixmap);
}

void MainWindow::update_points_table()
{
	ui->points_table->setRowCount(0);
	const auto &contours = polygon.get_contours();

	int total_rows = 0;
	for (const auto &contour : contours)
	{
		total_rows += contour.size();
	}
	ui->points_table->setRowCount(total_rows);

	int current_row = 0;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		for (const QPoint &point : contours[i])
		{
			ui->points_table->setItem(current_row, 0, new QTableWidgetItem(QString::number(point.x())));
			ui->points_table->setItem(current_row, 1, new QTableWidgetItem(QString::number(point.y())));
			current_row++;
		}
	}

	if (current_row < total_rows)
		ui->points_table->setRowCount(current_row);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if (!ui->graphics_view->rect().contains(ui->graphics_view->mapFromGlobal(event->globalPos())))
		return;

	QPointF scene_pos = ui->graphics_view->mapToScene(ui->graphics_view->mapFromGlobal(event->globalPos()));
	int x = static_cast<int>(scene_pos.x());
	int y = static_cast<int>(scene_pos.y());

	if (x < 0 || x >= scene->width() || y < 0 || y >= scene->height())
	{
		ui->label_4->setText("Point out of bounds.");
		return;
	}

	const auto &contours = polygon.get_contours();
	if (!contours.back().empty())
	{
		const QPoint &last = contours.back().back();
		if (qAbs(x - last.x()) < 5) x = last.x();
		if (qAbs(y - last.y()) < 5) y = last.y();
	}

	if (event->button() == Qt::LeftButton)
	{
		polygon.add_point(QPoint(x, y));
		draw_polygon();
		update_points_table();
	}
	else if (event->button() == Qt::RightButton)
	{
		if (contours.back().size() >= 3)
		{
			polygon.close_current_contour();
			draw_polygon();
			update_points_table();
		}
		else
		{
			ui->label_4->setText("Недостаточно точек для замыкания контуров.");
		}
	}
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);

	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);
	draw_polygon();
}


void MainWindow::on_draw_point_btn_clicked()
{
	bool ok_x, ok_y;
	double x = ui->x_coord_edit->text().toDouble(&ok_x);
	double y = ui->y_coord_edit->text().toDouble(&ok_y);

	if (!ok_x || !ok_y)
	{
		ui->label_4->setText("Invalid coordinates.");
		return;
	}

	int ix = static_cast<int>(x);
	int iy = static_cast<int>(y);

	if (ix >= 0 && ix < scene->width() && iy >= 0 && iy < scene->height())
	{
		polygon.add_point(QPoint(ix, iy));
		draw_polygon();
		update_points_table();
	}
	else
	{
		ui->label_4->setText("Coordinates out of bounds.");
	}
}

void MainWindow::on_close_figure_btn_clicked()
{
	if (polygon.get_contours().back().size() >= 3)
	{
		polygon.close_current_contour();
		draw_polygon();
		update_points_table();
	}
	else
	{
		ui->label_4->setText("Недостаточно точек для замыкания контуров.");
	}
}

void MainWindow::on_clear_btn_clicked()
{
	polygon.clear();
	pixmap.fill(Qt::transparent);
	scene->clear();
	scene->addPixmap(pixmap);
	update_points_table();
	ui->label_4->setText("");
}

void MainWindow::on_draw_btn_clicked()
{
	if (!polygon.has_closed_contours())
	{
		ui->label_4->setText("Нет замкнутых контуров для заполнения.");
		return;
	}

	pixmap.fill(Qt::transparent);

	// Отрисовка рёбер
	QPainter edge_painter(&pixmap);
	edge_painter.setPen(figure_color);
	QList<Edge> edges = polygon.create_edges();
	for (const Edge &edge : edges)
	{
		edge_painter.drawLine(edge.start, edge.end);
	}
	edge_painter.end();

	scene->clear();
	scene->addPixmap(pixmap);
	QApplication::processEvents();

	QPainter fill_painter(&pixmap);
	fill_painter.setPen(figure_color);

	if (delay_mode)
	{
		ui->label_4->setText("Визуализация заполнения...");
		QApplication::processEvents();
	}

	qint64 execution_time;

	polygon.fill_polygon(fill_painter, figure_color, delay_mode, execution_time, scene);
	fill_painter.end();

	scene->clear();
	scene->addPixmap(pixmap);

	if (!delay_mode)
		ui->label_4->setText(QString("Заполнение заняло %1 мс").arg(execution_time));
	else
		ui->label_4->setText(QString("Визуализация заполнения завершена за %1 мс").arg(execution_time));
}

void MainWindow::on_change_back_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(background_color, this, "Select Background Color");
	if (new_color.isValid())
	{
		background_color = new_color;
		set_preview_color(ui->background_color_widget, background_color);
		scene->setBackgroundBrush(background_color);
		draw_polygon();
	}
}

void MainWindow::on_change_figure_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(figure_color, this, "Select Figure Color");
	if (new_color.isValid())
	{  // Check if user didn't cancel
		figure_color = new_color;
		set_preview_color(ui->line_color_widget, figure_color);
		draw_polygon();
	}
}

void MainWindow::on_delay_radio_btn_toggled(bool checked)
{
	if (checked)
		delay_mode = true;
}

void MainWindow::on_without_delay_radio_btn_toggled(bool checked)
{
	if (checked)
		delay_mode = false;
}