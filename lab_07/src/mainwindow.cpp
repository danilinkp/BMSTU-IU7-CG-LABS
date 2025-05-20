#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDoubleValidator>
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("Отсекатели");

	auto *validator = new QDoubleValidator(this);
	validator->setLocale(QLocale::C);
	ui->x_start_edit->setValidator(validator);
	ui->y_start_edit->setValidator(validator);
	ui->x_end_edit->setValidator(validator);
	ui->y_end_edit->setValidator(validator);

	ui->x_left_edit->setValidator(validator);
	ui->y_left_edit->setValidator(validator);
	ui->x_right_edit->setValidator(validator);
	ui->y_right_edit->setValidator(validator);


	scene = new QGraphicsScene(this);
	ui->graphics_view->setScene(scene);
	scene->setBackgroundBrush(Qt::white);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
	pixmap.fill(Qt::transparent);
	scene->addPixmap(pixmap);

	drawer = new Drawer(scene, pixmap);

	set_preview_color(ui->section_color_widget, section_color);
	set_preview_color(ui->clipper_color_widget, clipper_color);
	set_preview_color(ui->result_color_widget, result_color);


	ui->graphics_view->viewport()->installEventFilter(this);
	ui->graphics_view->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete drawer;
	delete clipper;
}

void MainWindow::set_preview_color(QWidget *widget, const QColor &color)
{
	QPalette palette = widget->palette();
	palette.setColor(QPalette::Window, color);
	widget->setPalette(palette);
	widget->setAutoFillBackground(true);
}

void MainWindow::on_change_section_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(section_color, this, "Select Section Color");
	if (new_color.isValid())
	{
		section_color = new_color;
		set_preview_color(ui->section_color_widget, section_color);
	}
}

void MainWindow::on_change_clipper_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(clipper_color, this, "Select Clipper Color");
	if (new_color.isValid())
	{
		clipper_color = new_color;
		set_preview_color(ui->clipper_color_widget, clipper_color);
	}
}

void MainWindow::on_change_result_color_btn_clicked()
{
	QColor new_color = QColorDialog::getColor(result_color, this, "Select Result Color");
	if (new_color.isValid())
	{
		result_color = new_color;
		set_preview_color(ui->result_color_widget, result_color);
	}
}

void MainWindow::on_clear_btn_clicked()
{
	segments.clear();
	clipped_segments.clear();
	delete clipper;
	clipper = nullptr;
	input_mode = InputMode::None;
	pixmap.fill(Qt::transparent);
	scene->clear();
	scene->addPixmap(pixmap);
	drawer->clear();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	QPointF global_pos = event->globalPosition();
	QPoint pos = ui->graphics_view->mapFromGlobal(global_pos.toPoint());
	if (!ui->graphics_view->rect().contains(pos))
		return;

	if (event->button() == Qt::LeftButton)
	{
		if (input_mode == InputMode::None || input_mode == InputMode::SegmentSecond)
		{
			first_point = pos;
			input_mode = InputMode::SegmentFirst;
		}
		else if (input_mode == InputMode::SegmentFirst)
		{
			QPoint adjusted_pos = pos;
			if (event->modifiers() & Qt::ControlModifier)
			{
				int dx = qAbs(pos.x() - first_point.x());
				int dy = qAbs(pos.y() - first_point.y());
				if (dx > dy)
					adjusted_pos.setY(first_point.y());
				else
					adjusted_pos.setX(first_point.x());
			}
			LineSegment segment(first_point, adjusted_pos);
			segments.push_back(segment);
			drawer->draw_line(first_point, adjusted_pos, section_color);
			drawer->render();
			input_mode = InputMode::None;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (input_mode == InputMode::None || input_mode == InputMode::ClipperSecond)
		{
			first_point = pos;
			input_mode = InputMode::ClipperFirst;
		}
		else if (input_mode == InputMode::ClipperFirst)
		{
			clipped_segments.clear();
			delete clipper;
			clipper = new ClipperRectangle(first_point, pos);
			int x_left = std::min(first_point.x(), pos.x());
			int x_right = std::max(first_point.x(), pos.x());
			int y_top = std::min(first_point.y(), pos.y());
			int y_bottom = std::max(first_point.y(), pos.y());
			ui->x_left_edit->setText(QString("%1").arg(x_left));
			ui->y_left_edit->setText(QString("%1").arg(y_top));
			ui->x_right_edit->setText(QString("%1").arg(x_right));
			ui->y_right_edit->setText(QString("%1").arg(y_bottom));
			redraw_scene();
			input_mode = InputMode::None;
		}
	}
}

void MainWindow::redraw_scene(bool is_cliped)
{
	drawer->clear();
	for (const auto &segment : segments)
		drawer->draw_line(segment.start(), segment.end(), section_color);

	if (clipper)
	{
		if (is_cliped)
		{
			auto rectangle_clipper = dynamic_cast<ClipperRectangle*>(clipper);
			if (rectangle_clipper)
				drawer->fill_rectangle(rectangle_clipper->get_rectangle(), Qt::white);
		}
		clipper->draw(*drawer, clipper_color);
	}

	for (const auto &clipped_segment : clipped_segments)
		drawer->draw_line(clipped_segment.start(), clipped_segment.end(), result_color);
	drawer->render();
}

void MainWindow::on_clip_btn_clicked()
{
	if (clipper && !segments.empty())
	{
		Clipper clipper_obj(clipper);
		clipped_segments = clipper_obj.clip_segments(segments);
		redraw_scene(true);
	}
}

void MainWindow::on_draw_section_btn_clicked()
{
	int x_start = ui->x_start_edit->text().toInt();
	int y_start = ui->y_start_edit->text().toInt();
	QPoint start(x_start, y_start);

	int x_end = ui->x_end_edit->text().toInt();
	int y_end = ui->y_end_edit->text().toInt();
	QPoint end(x_end, y_end);

	LineSegment segment(start, end);
	segments.push_back(segment);
	drawer->draw_line(start, end, section_color);
	drawer->render();
}

void MainWindow::on_draw_clipper_btn_clicked()
{
	int x_left = ui->x_left_edit->text().toInt();
	int y_left = ui->y_left_edit->text().toInt();
	QPoint left(x_left, y_left);

	int x_right = ui->x_right_edit->text().toInt();
	int y_right = ui->y_right_edit->text().toInt();
	QPoint right(x_right, y_right);

	clipped_segments.clear();
	delete clipper;
	clipper = new ClipperRectangle(left, right);
	redraw_scene();
}
