#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDoubleValidator>
#include <QMessageBox>
#include <iostream>
#include <QListWidgetItem>
#include <QComboBox>

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

	ui->x_clipper_edit->setValidator(validator);
	ui->y_clipper_edit->setValidator(validator);

	ui->points_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui->graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphics_view->setDragMode(QGraphicsView::NoDrag);
	ui->graphics_view->setRenderHint(QPainter::Antialiasing);
	ui->graphics_view->setRenderHint(QPainter::SmoothPixmapTransform);

	scene = new QGraphicsScene(this);
	ui->graphics_view->setScene(scene);
	ui->graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scene->setBackgroundBrush(Qt::white);
	scene->setSceneRect(0, 0, ui->graphics_view->width(), ui->graphics_view->height());
	ui->graphics_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	pixmap = QPixmap(static_cast<int>(scene->width()), static_cast<int>(scene->height()) + 20);
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
	clipper_points.clear();
	delete clipper;
	clipper = nullptr;
	input_mode = InputMode::None;
	ui->points_table->clearContents();
	ui->points_table->setRowCount(0);
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
			LineSegment segment(first_point, pos);
			segments.push_back(segment);
			drawer->draw_line(first_point, pos, section_color);
			drawer->render();
			input_mode = InputMode::None;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (input_mode == InputMode::None || input_mode == InputMode::ClipperSecond)
		{
			if (input_mode == InputMode::None)
			{
				clipped_segments.clear();
				ui->points_table->setRowCount(0);
				clipper_points.clear();
				delete clipper;
				clipper = nullptr;
				redraw_scene();
			}
			int row = ui->points_table->rowCount();
			ui->points_table->insertRow(row);
			ui->points_table->setItem(row, 0, new QTableWidgetItem(QString::number(pos.x())));
			ui->points_table->setItem(row, 1, new QTableWidgetItem(QString::number(pos.y())));
			clipper_points.push_back(pos);
			if (clipper_points.size() == 1)
			{
				first_point = pos;
				last_pos = pos;
				input_mode = InputMode::ClipperFirst;
			}
			else
			{
				drawer->draw_line(last_pos, pos, clipper_color);
				drawer->render();
				last_pos = pos;
				input_mode = InputMode::ClipperSecond;
			}
		}
		else if (input_mode == InputMode::ClipperFirst)
		{
			int row = ui->points_table->rowCount();
			ui->points_table->insertRow(row);
			ui->points_table->setItem(row, 0, new QTableWidgetItem(QString::number(pos.x())));
			ui->points_table->setItem(row, 1, new QTableWidgetItem(QString::number(pos.y())));
			clipper_points.push_back(pos);
			drawer->draw_line(last_pos, pos, clipper_color);
			drawer->render();
			last_pos = pos;
			input_mode = InputMode::ClipperSecond;
		}
	}
	else if (event->button() == Qt::MiddleButton)
	{
		if (clipper_points.size() < 3)
		{
			QMessageBox::warning(this, "Ошибка", "Недостаточно точек для замыкания отсекателя. Нужно минимум 3 точки.");
			return;
		}

		ClipperPolygon temp_clipper(clipper_points);
		if (!temp_clipper.is_convex())
		{
			QMessageBox::warning(this, "Ошибка", "Многоугольник должен быть выпуклым.");
			clipped_segments.clear();
			ui->points_table->setRowCount(0);
			clipper_points.clear();
			delete clipper;
			clipper = nullptr;
			input_mode = InputMode::None;
			redraw_scene();
			return;
		}

		drawer->draw_line(last_pos, first_point, clipper_color);
		drawer->render();
		delete clipper;
		clipper = new ClipperPolygon(clipper_points);
		Clipper clipper_obj(clipper);
		clipped_segments = clipper_obj.clip_segments(segments);
		redraw_scene();
		input_mode = InputMode::None;
	}
}

void MainWindow::redraw_scene()
{
	drawer->clear();
	for (const auto &segment : segments)
		drawer->draw_line(segment.start(), segment.end(), section_color);

	if (clipper)
		clipper->draw(*drawer, clipper_color);

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
		if (!clipper->is_convex())
			QMessageBox::warning(this, "Ошибка", "Многоугольник должен быть выпуклым.");
		else
			redraw_scene();
	}
}

void MainWindow::on_draw_section_btn_clicked()
{
	int x_start = ui->x_start_edit->text().toInt();
	int y_start = ui->y_start_edit->text().toInt();
	QPoint start(x_start, y_start + 1);

	int x_end = ui->x_end_edit->text().toInt();
	int y_end = ui->y_end_edit->text().toInt();
	QPoint end(x_end, y_end + 1);

	LineSegment segment(start, end);
	segments.push_back(segment);
	drawer->draw_line(start, end, section_color);
	drawer->render();
}

void MainWindow::on_draw_clipper_btn_clicked()
{
	int x = ui->x_clipper_edit->text().toInt();
	int y = ui->y_clipper_edit->text().toInt();
	QPoint pos(x, y);


	if (input_mode == InputMode::None || input_mode == InputMode::ClipperSecond)
	{
		if (input_mode == InputMode::None)
		{
			clipped_segments.clear();
			ui->points_table->setRowCount(0);
			clipper_points.clear();
			delete clipper;
			clipper = nullptr;
			redraw_scene();
		}
		int row = ui->points_table->rowCount();
		ui->points_table->insertRow(row);
		ui->points_table->setItem(row, 0, new QTableWidgetItem(QString::number(pos.x())));
		ui->points_table->setItem(row, 1, new QTableWidgetItem(QString::number(pos.y())));
		clipper_points.push_back(pos);
		if (clipper_points.size() == 1)
		{
			first_point = pos;
			last_pos = pos;
			input_mode = InputMode::ClipperFirst;
		}
		else
		{
			drawer->draw_line(last_pos, pos, clipper_color);
			drawer->render();
			last_pos = pos;
			input_mode = InputMode::ClipperSecond;
		}
	}
	else if (input_mode == InputMode::ClipperFirst)
	{
		int row = ui->points_table->rowCount();
		ui->points_table->insertRow(row);
		ui->points_table->setItem(row, 0, new QTableWidgetItem(QString::number(pos.x())));
		ui->points_table->setItem(row, 1, new QTableWidgetItem(QString::number(pos.y())));
		clipper_points.push_back(pos);
		drawer->draw_line(last_pos, pos, clipper_color);
		drawer->render();
		last_pos = pos;
		input_mode = InputMode::ClipperSecond;
	}
}

void MainWindow::on_close_clipper_btn_clicked()
{
	if (clipper_points.size() < 3)
		QMessageBox::warning(this, "Ошибка", "Недостаточно точек для замыкания отсекателя. Нужно минимум 3 точки.");
	else
	{
		drawer->draw_line(last_pos, first_point, clipper_color);
		drawer->render();
		delete clipper;
		clipper = new ClipperPolygon(clipper_points);
		redraw_scene();
		input_mode = InputMode::None;
	}
}

void MainWindow::on_parallel_sections_btn_clicked()
{
	if (!clipper || clipper_points.size() < 3)
	{
		QMessageBox::warning(this, "Ошибка", "Сначала создайте отсекатель (минимум 3 точки).");
		return;
	}

	QDialog dialog(nullptr);

	QFont font = dialog.font();
	font.setPointSize(14);

	dialog.setFont(font);
	dialog.setWindowTitle("Параллельные отрезки");
	dialog.setModal(true);
	dialog.resize(600, 400);

	auto *layout = new QVBoxLayout(&dialog);

	auto *sidesLabel = new QLabel("Выберите стороны отсекателя:");
	layout->addWidget(sidesLabel);

	auto *sidesListWidget = new QListWidget();
	sidesListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

	for (size_t i = 0; i < clipper_points.size(); ++i)
	{
		size_t next_i = (i + 1) % clipper_points.size();
		QString sideText = QString("Сторона %1: (%2,%3) - (%4,%5)")
			.arg(i + 1)
			.arg(clipper_points[i].x())
			.arg(clipper_points[i].y())
			.arg(clipper_points[next_i].x())
			.arg(clipper_points[next_i].y());
		sidesListWidget->addItem(sideText);
	}
	layout->addWidget(sidesListWidget);

	auto *distanceLabel = new QLabel("Расстояние от стороны:");
	layout->addWidget(distanceLabel);

	auto *distanceEdit = new QLineEdit();
	distanceEdit->setText("50");
	auto *validator = new QDoubleValidator(1.0, 1000.0, 2, &dialog);
	validator->setLocale(QLocale::C);
	distanceEdit->setValidator(validator);
	layout->addWidget(distanceEdit);

	auto *directionLabel = new QLabel("Направление:");
	layout->addWidget(directionLabel);

	auto *directionCombo = new QComboBox();
	directionCombo->addItem("Внутрь отсекателя", 0);
	directionCombo->addItem("Наружу отсекателя", 1);
	layout->addWidget(directionCombo);

	auto *buttonLayout = new QHBoxLayout();
	auto *okButton = new QPushButton("OK");
	auto *cancelButton = new QPushButton("Отмена");
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	layout->addLayout(buttonLayout);

	connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

	if (dialog.exec() == QDialog::Accepted)
	{
		QList<QListWidgetItem *> selectedItems = sidesListWidget->selectedItems();
		if (selectedItems.isEmpty())
		{
			QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одну сторону.");
			return;
		}

		double distance = distanceEdit->text().toDouble();
		bool outward = directionCombo->currentData().toInt() == 1;

		for (QListWidgetItem *item : selectedItems)
		{
			int sideIndex = sidesListWidget->row(item);
			createParallelSegment(sideIndex, distance, outward);
		}

		drawer->render();
	}
}

void MainWindow::createParallelSegment(int sideIndex, double distance, bool outward)
{
	if (sideIndex < 0 || sideIndex >= static_cast<int>(clipper_points.size()))
		return;

	size_t current = static_cast<size_t>(sideIndex);
	size_t next = (current + 1) % clipper_points.size();

	QPoint p1 = clipper_points[current];
	QPoint p2 = clipper_points[next];

	QVector2D sideVector(p2.x() - p1.x(), p2.y() - p1.y());

	sideVector.normalize();

	QVector2D normalVector(-sideVector.y(), sideVector.x());

	QPoint center = calculatePolygonCenter();
	QPoint midPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);

	QVector2D toCenter(center.x() - midPoint.x(), center.y() - midPoint.y());

	bool normalPointsOutward = QVector2D::dotProduct(normalVector, toCenter) < 0;

	if (outward != normalPointsOutward)
		normalVector = -normalVector;

	QPoint parallelP1(
		static_cast<int>(p1.x() + normalVector.x() * distance),
		static_cast<int>(p1.y() + normalVector.y() * distance)
	);

	QPoint parallelP2(
		static_cast<int>(p2.x() + normalVector.x() * distance),
		static_cast<int>(p2.y() + normalVector.y() * distance)
	);

	LineSegment parallelSegment(parallelP1, parallelP2);
	segments.push_back(parallelSegment);

	drawer->draw_line(parallelP1, parallelP2, section_color);
}

QPoint MainWindow::calculatePolygonCenter()
{
	if (clipper_points.empty())
		return {0, 0};

	int sumX = 0, sumY = 0;
	for (const QPoint &point : clipper_points)
	{
		sumX += point.x();
		sumY += point.y();
	}

	return {sumX / static_cast<int>(clipper_points.size()),
			sumY / static_cast<int>(clipper_points.size())};
}