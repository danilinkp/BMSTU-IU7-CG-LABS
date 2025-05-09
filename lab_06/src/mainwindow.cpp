#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDoubleValidator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("Polygon Filling with Holes");

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
	scene->addPixmap(pixmap);

	drawer = new Drawer(scene, pixmap);

	set_preview_color(ui->background_color_widget, background_color);
	set_preview_color(ui->line_color_widget, figure_color);
	set_preview_color(ui->border_color_widget, border_color);

	ui->without_delay_radio_btn->setChecked(true);
	delay_mode = false;

	seed_pos = QPoint(ui->graphics_view->width() / 2, ui->graphics_view->height() / 2);
	ui->seed_label->setText(QString("Координаты затравки: x: %1, y: %2").arg(seed_pos.x()).arg(seed_pos.y()));

	ui->graphics_view->viewport()->installEventFilter(this);
	ui->graphics_view->setMouseTracking(true);
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

void MainWindow::on_change_back_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(background_color, this, "Select Background Color");
	if (new_color.isValid())
	{
		background_color = new_color;
		set_preview_color(ui->background_color_widget, background_color);
		scene->setBackgroundBrush(background_color);
	}
}

void MainWindow::on_change_figure_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(figure_color, this, "Select Figure Color");
	if (new_color.isValid())
	{
		figure_color = new_color;
		set_preview_color(ui->line_color_widget, figure_color);
	}
}

void MainWindow::on_change_border_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(border_color, this, "Select Border Color");
	if (new_color.isValid())
	{
		border_color = new_color;
		set_preview_color(ui->border_color_widget, border_color);
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

void MainWindow::on_clear_btn_clicked()
{
	has_last_pos = false;
	has_first_pos = false;
	pixmap.fill(Qt::transparent);
	scene->clear();
	scene->addPixmap(pixmap);
	drawer->clear();
	ui->label_4->setText("");
	ui->points_table->clearContents();
	ui->points_table->setRowCount(0);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == ui->graphics_view->viewport())
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
			QPoint current_pos = ui->graphics_view->mapToScene(mouseEvent->pos()).toPoint();

			if (mouseEvent->button() == Qt::LeftButton)
			{
				ui->points_table->insertRow(ui->points_table->rowCount());
				ui->points_table->setItem(ui->points_table->rowCount() - 1,
										  0,
										  new QTableWidgetItem(QString::number(current_pos.x())));
				ui->points_table->setItem(ui->points_table->rowCount() - 1,
										  1,
										  new QTableWidgetItem(QString::number(current_pos.y())));
				is_drawing = true;
				if (!has_first_pos)
				{
					first_pos = current_pos;
					has_first_pos = true;
					current_figure = Figure();
					current_figure.add_point(current_pos);
				}
				else
					current_figure.add_point(current_pos);

				if (has_last_pos)
					drawer->draw_line(last_pos, current_pos, border_color);
				else
					drawer->draw_pixel(current_pos, border_color);

				last_pos = current_pos;
				has_last_pos = true;
				drawer->render();
				return true;
			}
			else if (mouseEvent->button() == Qt::RightButton)
			{
				if (has_first_pos && has_last_pos)
				{
					if (current_figure.get_points_count() < 3)
						QMessageBox::warning(this,
											 "Ошибка",
											 "Недостаточно точек для замыкания фигуры. Нужно минимум 3 точки.");
					else
					{
						drawer->draw_line(last_pos, first_pos, border_color);
						current_figure.add_point(first_pos);
						current_figure.close();
						figures.push_back(current_figure);
						drawer->render();
						current_figure = Figure();
						has_first_pos = false;
						has_last_pos = false;
					}

				}
				return true;
			}
			else if (mouseEvent->button() == Qt::MiddleButton)
			{
				seed_pos = current_pos;
				ui->seed_label
					->setText(QString("Координаты затравки: x: %1, y: %2").arg(seed_pos.x()).arg(seed_pos.y()));
			}
		}
		else if (event->type() == QEvent::MouseMove)
		{
			auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
			if (is_drawing)
			{
				QPoint current_pos = ui->graphics_view->mapToScene(mouseEvent->pos()).toPoint();
				ui->points_table->insertRow(ui->points_table->rowCount());
				ui->points_table->setItem(ui->points_table->rowCount() - 1,
										  0,
										  new QTableWidgetItem(QString::number(current_pos.x())));
				ui->points_table->setItem(ui->points_table->rowCount() - 1,
										  1,
										  new QTableWidgetItem(QString::number(current_pos.y())));
				drawer->draw_line(last_pos, current_pos, border_color);
				current_figure.add_point(current_pos);
				last_pos = current_pos;
				drawer->render();
				return true;
			}
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
			if (mouseEvent->button() == Qt::LeftButton)
			{
				is_drawing = false;
				return true;
			}
		}
	}
	return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_draw_point_btn_clicked()
{
	QPoint current_pos = QPoint(ui->x_coord_edit->text().toInt(), ui->y_coord_edit->text().toInt());
	QMessageBox::warning(this,
						 "Ошибка",
						 "Недостаточно точек для замыкания фигуры. Нужно минимум 3 точки.");

	ui->points_table->insertRow(ui->points_table->rowCount());
	ui->points_table->setItem(ui->points_table->rowCount() - 1,
							  0,
							  new QTableWidgetItem(QString::number(current_pos.x())));
	ui->points_table->setItem(ui->points_table->rowCount() - 1,
							  1,
							  new QTableWidgetItem(QString::number(current_pos.y())));
	is_drawing = true;
	if (!has_first_pos)
	{
		first_pos = current_pos;
		has_first_pos = true;
		current_figure = Figure();
		current_figure.add_point(current_pos);
	}
	else
		current_figure.add_point(current_pos);

	if (has_last_pos)
		drawer->draw_line(last_pos, current_pos, border_color);
	else
		drawer->draw_pixel(current_pos, border_color);

	last_pos = current_pos;
	has_last_pos = true;
	drawer->render();
}

void MainWindow::add_seed_btn_clicked()
{
	QPoint current_pos = QPoint(ui->x_coord_edit->text().toInt(), ui->y_coord_edit->text().toInt());
	if (current_pos.x() < 0 || current_pos.x() >= pixmap.width() || current_pos.y() < 0
		|| current_pos.y() >= pixmap.height())
		QMessageBox::warning(this,
							 "Ошибка",
							 "Точка выходит за пределы области рисования.");

	seed_pos = current_pos;
	ui->seed_label->setText(QString("Координаты затравки: x: %1, y: %2").arg(seed_pos.x()).arg(seed_pos.y()));
}

void MainWindow::on_close_figure_btn_clicked()
{
	if (has_first_pos && has_last_pos)
	{
		if (current_figure.get_points_count() < 3)
			QMessageBox::warning(this,
								 "Ошибка",
								 "Недостаточно точек для замыкания фигуры. Нужно минимум 3 точки.");
		else
		{
			drawer->draw_line(last_pos, first_pos, border_color);
			current_figure.add_point(first_pos);
			current_figure.close();
			figures.push_back(current_figure);
			drawer->render();
		}
		current_figure = Figure();
		has_first_pos = false;
		has_last_pos = false;
	}
}

void MainWindow::on_draw_btn_clicked()
{
	if (current_figure.get_points_count() > 0 && !current_figure.is_closed())
		QMessageBox::warning(this,
							 "Ошибка",
							 "Нужно замкнуть фигуру.");
	else
	{
		QElapsedTimer timer;
		timer.start();

		scanline_seed_fill(drawer, seed_pos, border_color, figure_color, delay_mode);

		auto time = timer.elapsed();

		if (!delay_mode)
			ui->label_4->setText(QString("Время выполнения: %1 мс").arg(time));
	}
}