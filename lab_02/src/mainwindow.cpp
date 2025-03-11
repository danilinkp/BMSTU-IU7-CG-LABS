#include <QDoubleValidator>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), graphicsManager(new GraphicsManager(this))
{
	ui->setupUi(this);
	setFixedSize(1550, 900);

	ui->graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	graphicsManager->set_graphics_view(ui->graphics_view);
	graphicsManager->draw_initial_figure();

	show_center_figure();
	auto validator = new QDoubleValidator;
	validator->setLocale(QLocale::C);

	ui->dx_line_edit->setValidator(validator);
	ui->dy_line_edit->setValidator(validator);
	ui->kx_line_edit->setValidator(validator);
	ui->ky_line_edit->setValidator(validator);
	ui->cx_line_edit->setValidator(validator);
	ui->cy_line_edit->setValidator(validator);
	ui->angle_line_edit->setValidator(validator);


	ui->dx_line_edit->setText("0");
	ui->dy_line_edit->setText("0");
	ui->kx_line_edit->setText("1");
	ui->ky_line_edit->setText("1");
	ui->cx_line_edit->setText("0");
	ui->cy_line_edit->setText("0");
	ui->angle_line_edit->setText("0");
}

MainWindow::~MainWindow()
{
	delete ui;
	delete graphicsManager;
}

void MainWindow::on_move_button_clicked()
{
	bool ok1, ok2;
	double dx = ui->dx_line_edit->text().toDouble(&ok1);
	double dy = ui->dy_line_edit->text().toDouble(&ok2);
	if (ok1 && ok2)
	{
		graphicsManager->move(dx, dy);
		show_center_figure();
		ui->statusbar->showMessage("Перемещение выполнено", 3000);
	}
	else
		ui->statusbar->showMessage("Ошибка: Неверные значения dx или dy", 3000);
}

void MainWindow::on_scale_button_clicked()
{
	bool ok1, ok2, ok3, ok4;
	double kx = ui->kx_line_edit->text().toDouble(&ok1);
	double ky = ui->ky_line_edit->text().toDouble(&ok2);
	double cx = ui->cx_line_edit->text().toDouble(&ok3);
	double cy = ui->cy_line_edit->text().toDouble(&ok4);
	if (ok1 && ok2 && ok3 && ok4 && kx != 0 && ky != 0)
	{
		graphicsManager->scale(kx, ky, cx, cy);
		show_center_figure();
		ui->statusbar->showMessage("Масштабирование выполнено", 3000);
	}
	else
	{
		ui->statusbar->showMessage("Ошибка: Неверные значения kx, ky, cx или cy", 3000);
	}
}

void MainWindow::on_rotate_button_clicked()
{
	bool ok1, ok2, ok3;
	double angle = ui->angle_line_edit->text().toDouble(&ok1);
	double rx = ui->cx_line_edit->text().toDouble(&ok2);
	double ry = ui->cy_line_edit->text().toDouble(&ok3);
	if (ok1 && ok2 && ok3)
	{
		graphicsManager->rotate(angle, rx, ry);
		show_center_figure();
		ui->statusbar->showMessage("Поворот выполнен", 3000);
	}
	else
		ui->statusbar->showMessage("Ошибка: Неверные значения угла, rx или ry", 3000);
}

void MainWindow::on_reset_button_clicked()
{
	graphicsManager->reset();
	show_center_figure();
	ui->statusbar->showMessage("Сброшено в начальное состояние", 3000);
}

void MainWindow::on_back_button_clicked()
{
	graphicsManager->undo();
	show_center_figure();
	ui->statusbar->showMessage("Отменено последнее действие", 3000);
}

void MainWindow::show_center_figure()
{
	QString center_str =
		QString("Центр фигуры: (%1; %2)").arg(graphicsManager->get_intersection_center().x())
			.arg(-graphicsManager->get_intersection_center().y());
	ui->center_label->setText(center_str);
}
