#ifndef SCANLINE_SEED_FILL_H
#define SCANLINE_SEED_FILL_H

#include "drawer.h"

void scanline_seed_fill(Drawer *drawer,
						const QPoint &seed_point,
						const QColor &border_color,
						const QColor &fill_color,
						bool delay);

#endif //SCANLINE_SEED_FILL_H
