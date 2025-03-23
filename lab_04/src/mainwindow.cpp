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
	setWindowTitle("4-ая лаба");

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

