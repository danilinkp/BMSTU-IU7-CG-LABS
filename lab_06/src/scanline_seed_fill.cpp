#include <QStack>
#include <QThread>
#include <QApplication>
#include "scanline_seed_fill.h"

void scanline_seed_fill(Drawer *drawer,
						const QPoint &seed_point,
						const QColor &border_color,
						const QColor &fill_color,
						bool delay)
{
	QStack<QPoint> stack;
	stack.push(seed_point);

	QPoint point;
	int x, y, x_left, x_right;
	while (!stack.empty())
	{
		point = stack.pop();
		x = point.x();
		y = point.y();

		// соххраняем абсциссу точки
		int wx = x;

		drawer->draw_pixel(point, fill_color);

		// заполняем пиксели справа от точки
		x++;
		while (x < drawer->get_pixmap_width() && drawer->get_pixel_color(x, y) != border_color)
		{
			drawer->draw_pixel(x, y, fill_color);
			x++;
		}

		// сохраняем абсциссу крайнего правого пикселя
		x_right = x - 1;

		x = wx;

		x--;
		// заполняем пиксели слева от точки
		while (x >= 0 && drawer->get_pixel_color(x, y) != border_color)
		{
			drawer->draw_pixel(x, y, fill_color);
			x--;
		}
		x_left = x + 1; // сохраняем абсциссу крайнего левого пикселя

		x = x_left;
		++y;

		// ищем затравочные пиксели сверху
		if (y < drawer->get_pixmap_height())
		{
			while (x <= x_right)
			{
				bool f = false;
				while (x <= x_right && drawer->get_pixel_color(x, y) != border_color
					&& drawer->get_pixel_color(x, y) != fill_color)
				{
					f = true;
					x++;
				}

				// помещаем крайне правый пиксель в стек
				if (f)
				{
					if (x == x_right && drawer->get_pixel_color(x, y) != fill_color
						&& drawer->get_pixel_color(x, y) != border_color)
						stack.push(QPoint(x, y));
					else
						stack.push(QPoint(x - 1, y));
					f = false;
				}

				wx = x;
				while (x < x_right
					&& (drawer->get_pixel_color(x, y) == border_color || drawer->get_pixel_color(x, y) == fill_color))
					x++;

				if (x == wx)
					x++;
			}
		}

		x = x_left;
		y -= 2;

		if (y >= 0)
		{
			while (x <= x_right)
			{
				bool f = false;
				while (x <= x_right && drawer->get_pixel_color(x, y) != border_color
					&& drawer->get_pixel_color(x, y) != fill_color)
				{
					f = true;
					x++;
				}
				if (f)
				{
					if (x == x_right && drawer->get_pixel_color(x, y) != fill_color
						&& drawer->get_pixel_color(x, y) != border_color)
						stack.push(QPoint(x, y));
					else
						stack.push(QPoint(x - 1, y));
					f = false;
				}

				wx = x;
				while (x < x_right
					&& (drawer->get_pixel_color(x, y) == border_color || drawer->get_pixel_color(x, y) == fill_color))
					x++;

				if (x == wx)
					x++;
			}
		}
		if (delay)
		{
			drawer->render();
			QApplication::processEvents();
			QThread::msleep(5);
		}
	}



	drawer->render();
}