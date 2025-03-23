#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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

private slots:
	void on_change_back_color_btn_clicked();
	void on_change_figure_color_btn_clicked();
private:
	Ui::MainWindow *ui;
	QGraphicsScene *scene;
	QPixmap pixmap;

	QColor background_color = Qt::white;
	QColor figure_color = Qt::red;

	static void set_preview_color(QWidget *widget, const QColor &color);

};

#endif // MAINWINDOW_H