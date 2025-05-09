#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "scanline_seed_fill.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow: public QMainWindow
{
Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

protected:
	bool eventFilter(QObject *obj, QEvent *event) override;


private slots:
	void on_clear_btn_clicked();
	void on_change_back_color_btn_clicked();
	void on_change_figure_color_btn_clicked();
	void on_change_border_color_btn_clicked();
	void on_delay_radio_btn_toggled(bool checked);
	void on_without_delay_radio_btn_toggled(bool checked);
	void on_draw_point_btn_clicked();
	void add_seed_btn_clicked();
	void on_close_figure_btn_clicked();
	void on_draw_btn_clicked();

private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;
	Drawer *drawer;
	QColor background_color = Qt::white;
	QColor figure_color = Qt::green;
	QColor border_color = Qt::black;
	bool delay_mode = false;

	QPoint seed_pos;

	bool is_drawing = false;
	QPoint last_pos;
	bool has_last_pos = false;
	QPoint first_pos;
	bool has_first_pos = false;

	Figure current_figure;
	std::vector<Figure> figures = {};
	static void set_preview_color(QWidget* widget, const QColor& color);
};

#endif // MAINWINDOW_H