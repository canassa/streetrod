/**
 * @file TANDYLIN.H
 * @brief Tandy line drawing and span fills.
 */

#ifndef TANDYLIN_H
#define TANDYLIN_H

void far tl_draw();
void tl_span(int x1, int x2, int y, int color);
void far tl_fill_below();

#endif
